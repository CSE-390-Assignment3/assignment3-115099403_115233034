#include <iostream>
// #include "Simulation/Simulator.h"
// #include "AlgorithmCommon/Algorithm_1.h"
#include "../Common/AlgorithmRegistrar.h"
#include <dlfcn.h>

#include "include/Config.h"
#include "include/InputParser.h"
#include "include/Simulator.h"

#include <thread>

using AlgorithmPtr = std::unique_ptr<AlgorithmRegistrar>;

struct AlgoParams {
  std::string file_name;
  bool dlopen_success = false;
  bool is_algo_valid = false;
  void *handle = nullptr;
  int factory_idx = -1;
};

struct SimParams {
  std::string file_name;
  Simulator sim;
  bool is_valid = false;
};

struct RunnableParams {
  std::string h_fname;
  std::string a_fname;
  int a_index;
};

CmdArgs cmd_args_;

bool loadTestHouseFiles(std::vector<SimParams> &simulators,
                        std::vector<std::string> house_files);

bool loadTestAlgoFiles(std::vector<AlgoParams> &algorithms,
                       std::vector<std::string> algo_files);

void worker(int t_id, std::vector<RunnableParams> runnable_params) {
  // std::cout << "Thread ID: " << std::this_thread::get_id() << " " << t_id <<
  // " "
  //           << runnable_params.size() << " \nnum_threads "
  //           << cmd_args_.num_threads << std::endl;
  int idx = t_id;
  auto get_fname = [=](auto hname, auto aname) {
    return getStem(hname) + "-" + getStem(aname) + ".txt";
  };
  while (idx < runnable_params.size()) {
    RunnableParams param = runnable_params[idx];
    Simulator sim;
    sim.readHouseFile(param.h_fname);
    auto algo =
        AlgorithmRegistrar::getAlgorithmRegistrar().begin() + param.a_index;
    auto algorithm = algo->create();
    sim.setAlgorithm(*algorithm);
    std::cout << get_fname(param.h_fname, param.h_fname) << std::endl;
    sim.run();
    if (!cmd_args_.summary_only)
      sim.dump(get_fname(param.h_fname, param.h_fname));
    idx += cmd_args_.num_threads;
  }
}

int main(int argc, char **argv) {
  // error handling not needed as all parameters have default value
  processArguments(argc, argv, cmd_args_); // != ArgumentsError::None;
  std::cout << "ArgumentsParsing Success!!" << std::endl;

  std::cout << "House path: " << cmd_args_.houses_path
            << ", Algo path: " << cmd_args_.algos_path
            << (cmd_args_.summary_only ? " and summary_only flag enabled" : "")
            << "\n number of threads " << cmd_args_.num_threads << std::endl;

  auto algo_files = parseDirectory(cmd_args_.algos_path, ALGO_EXTENSION_);
  auto house_files = parseDirectory(cmd_args_.houses_path, HOUSE_EXTENSION_);

  if (false) {
    std::cout << "DEBUG:: algo_files: \n";
    for (auto v : algo_files)
      std::cout << v << std::endl;
    std::cout << "DEBUG:: house_files: \n";
    for (auto v : house_files)
      std::cout << v << std::endl;
  }

  /* Init Simulator struct */
  std::vector<SimParams> simulators(house_files.size());
  /* Init Algo struct*/
  std::vector<AlgoParams> algorithms(algo_files.size());

  bool valid_simulator_found = false, valid_algo_found = true;

  valid_simulator_found = loadTestHouseFiles(simulators, house_files);

  valid_algo_found = loadTestAlgoFiles(algorithms, algo_files);

  if (!valid_algo_found) {
    std::cerr << "ArgumentsError No valid algo files found in path and current "
                 "directory, Stopping Simulator."
              << std::endl;
    return -1;
  }

  if (!valid_simulator_found) {
    std::cerr << "ArgumentsError No valid house files found in path and "
                 "current directory, Stopping Simulator."
              << std::endl;
    return -1;
  }

  std::cout << "AlgorithmRegistrar count "
            << AlgorithmRegistrar::getAlgorithmRegistrar().count() << std::endl;

  std::vector<RunnableParams> runnable_params;

  for (int aindex = 0; aindex < algorithms.size(); aindex++) {
    if (algorithms[aindex].is_algo_valid) {
      for (int hindex = 0; hindex < simulators.size(); hindex++) {
        if (simulators[hindex].is_valid) {
          RunnableParams param;
          param.h_fname = simulators[hindex].file_name;
          param.a_fname = algorithms[aindex].file_name;
          param.a_index = algorithms[aindex].factory_idx;
          runnable_params.push_back(std::move(param));

          // runnable_sims.back()->run();
          // runnable_sims.back().readHouseFile(simulators[hindex].file_name);
          // auto algo = AlgorithmRegistrar::getAlgorithmRegistrar().begin() +
          //             algorithms[aindex].factory_idx;
          // auto algorithm = algo->create();
          // runnable_sims.back().setAlgorithm(*algorithm);
        }
      }
    }
  }

  std::vector<std::thread> runnable_threads(cmd_args_.num_threads);

  for (int index = 0; index < cmd_args_.num_threads; index++) {
    runnable_threads[index] =
        std::thread(worker, index, std::ref(runnable_params));
  }

  for (int index = 0; index < cmd_args_.num_threads; index++) {
    runnable_threads[index].join();
  }

  algorithms.clear();
  AlgorithmRegistrar::getAlgorithmRegistrar().clear();

  for (int aindex = 0; aindex < algo_files.size(); aindex++) {
    if (algorithms[aindex].dlopen_success) {
      dlclose(algorithms[aindex].handle);
    }
  }
  return 0;
}

bool loadTestHouseFiles(std::vector<SimParams> &simulators,
                        std::vector<std::string> house_files) {
  bool found = false;
  for (int index = 0; index < house_files.size(); index++) {
    simulators[index].file_name = house_files[index];

    auto err = simulators[index].sim.readHouseFile(house_files[index]);
    if ((int)err < 0) {
      std::cerr << err << " at House File:" << house_files[index]
                << ". Skipping house file" << std::endl;

      std::ofstream out_error;
      out_error.open(getStem(simulators[index].file_name) +
                     std::string(ERROR_EXTENSION_));
      if (!out_error) {
        std::cout << "Error opening error file "
                  << getStem(simulators[index].file_name) +
                         std::string(ERROR_EXTENSION_)
                  << std::endl;
        std::cout << "Skipping writing to error file" << std::endl;
        continue;
      }
      out_error << err << std::endl;
      out_error << "File read error " << simulators[index].file_name
                << ". Skipping house file" << std::endl;
      out_error.close();
      continue;
    }

    simulators[index].is_valid = true;
    found = true;
    std::cout << "Simulator " << simulators[index].file_name
              << " loaded successfully " << std::endl;
  }
  return found;
}

bool loadTestAlgoFiles(std::vector<AlgoParams> &algorithms,
                       std::vector<std::string> algo_files) {
  bool found = false;
  for (int index = 0, valid_count = 0; index < algo_files.size(); index++) {
    auto registrar_count = AlgorithmRegistrar::getAlgorithmRegistrar().count();
    algorithms[index].file_name = algo_files[index];
    algorithms[index].handle =
        dlopen(algorithms[index].file_name.c_str(), RTLD_LAZY);
    if (!algorithms[index].handle) { // dlopen failed
      std::cout << "error algorithm dlopen " << algorithms[index].file_name
                << std::endl;
      std::ofstream out_error;
      out_error.open(getStem(algorithms[index].file_name) +
                     std::string(ERROR_EXTENSION_));
      out_error << "Error loading algorithm library: "
                << algorithms[index].file_name << " " << dlerror() << std::endl;
      out_error.close();
      continue;
    }
    algorithms[index].dlopen_success = true; // success

    if (AlgorithmRegistrar::getAlgorithmRegistrar().count() ==
        registrar_count) { // algorithm has not been registered
      std::ofstream out_error;
      out_error.open(getStem(algorithms[index].file_name) +
                     std::string(ERROR_EXTENSION_));
      out_error << "Algorithm registration failed: "
                << algorithms[index].file_name << std::endl;
      out_error.close();
    }
    auto algo =
        AlgorithmRegistrar::getAlgorithmRegistrar().begin() + valid_count;
    std::ofstream out_error;
    out_error.open(getStem(algorithms[index].file_name) +
                   std::string(ERROR_EXTENSION_));
    if (!out_error) {
      std::cout << "Error opening error file "
                << getStem(algorithms[index].file_name) +
                       std::string(ERROR_EXTENSION_)
                << std::endl;
      std::cout << "Skipping writing to error file" << std::endl;
    }
    try {
      std::cout << "creating algorithm " << index << " "
                << algorithms[index].file_name << std::endl;
      auto ptr = algo->create();
      if (!ptr) {
        out_error << "NULL algorithm created, skipping algorith \n";
        continue;
      }
      algorithms[index].factory_idx = valid_count++;
      algorithms[index].is_algo_valid = true;
      found = true;
    } catch (...) {
      if (out_error)
        out_error << "Algorithm create failed : " << algorithms[index].file_name
                  << std::endl;
    }
    if (out_error)
      out_error.close();
    std::cout << "Algorithm " << algorithms[index].file_name
              << " loaded successfully " << std::endl;
  }
  return found;
}
