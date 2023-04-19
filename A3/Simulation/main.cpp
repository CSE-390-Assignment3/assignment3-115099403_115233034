#include <iostream>
// #include "Simulation/Simulator.h"
// #include "AlgorithmCommon/Algorithm_1.h"
#include "../Common/AlgorithmRegistrar.h"
#include <dlfcn.h>

#include "include/Config.h"
#include "include/InputParser.h"
#include "include/Simulator.h"

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

void generateSummary(std::vector<SimParams> &sims,
                     std::vector<AlgoParams> &algos,
                     std::vector<std::vector<long>> &scores);

int main(int argc, char **argv) {
  std::string house_path = "", algo_path = "";
  bool summary_only = false;

  processArguments(argc, argv, house_path, algo_path, summary_only) !=
      ArgumentsError::None;
  std::cout << "ArgumentsParsing Success!!" << std::endl;

  std::cout << "House path: " << house_path << ", Algo path: " << algo_path
            << (summary_only ? " and summary_only flag enabled" : "")
            << std::endl;

  auto algo_files = parseDirectory(algo_path, ALGO_EXTENSION_);
  auto house_files = parseDirectory(house_path, HOUSE_EXTENSION_);

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

  std::vector<std::vector<long>> scores(algorithms.size(),
                                        std::vector<long>(simulators.size()));

  bool valid_simulator_found = false, valid_algo_found = true;

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
    valid_simulator_found = true;
    std::cout << "Simulator " << simulators[index].file_name
              << " loaded successfully " << std::endl;
  }

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
      valid_algo_found = true;
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

  auto get_fname = [=](auto hname, auto aname) {
    return getStem(hname) + "-" + getStem(aname) + ".txt";
  };

  for (int aindex = 0; aindex < algorithms.size(); aindex++) {
    if (algorithms[aindex].is_algo_valid) {
      for (int hindex = 0; hindex < simulators.size(); hindex++) {
        if (simulators[hindex].is_valid) {
          Simulator sim;
          sim.readHouseFile(simulators[hindex].file_name);
          auto algo = AlgorithmRegistrar::getAlgorithmRegistrar().begin() +
                      algorithms[aindex].factory_idx;
          auto algorithm = algo->create();
          sim.setAlgorithm(*algorithm);
          std::cout << get_fname(simulators[hindex].file_name,
                                 algorithms[aindex].file_name)
                    << std::endl;
          sim.run();
          if (!summary_only)
            sim.dump(get_fname(simulators[hindex].file_name,
                               algorithms[aindex].file_name));
          scores[aindex][hindex] = sim.getScore();
        }
      }
    }
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

void generateSummary(std::vector<SimParams> &sims,
                     std::vector<AlgoParams> &algos,
                     std::vector<std::vector<long>> &scores) {
  std::ofstream outfile("summary.csv");
  if (!outfile.is_open()) {
    std::cerr << "SummaryFileOpenError: Unable to create output file."
              << std::endl
              << "Stopping summary generation.";
    return;
  }

  std::stringstream outfile_row_ss;
  std::string outfile_row;

  outfile_row_ss << "Algo/House,";
  for (auto &sim : sims) {
    if (sim.is_valid)
      outfile_row_ss << sim.file_name << ",";
    std::cout << sim.file_name << ",";
  }
  outfile_row = outfile_row_ss.str();
  outfile_row.pop_back();
  std::cout << outfile_row << std::endl;
  outfile << outfile_row << std::endl;

  for (auto aindex = 0; aindex < algos.size(); aindex++) {
    if (algos[aindex].is_algo_valid) {

      outfile_row_ss.str(std::string()); // empty the string stream
      outfile_row_ss << algos[aindex].file_name << ",";

      for (auto sindex = 0; sindex < sims.size(); sindex++) {
        if (sims[sindex].is_valid) {
          outfile_row_ss << std::to_string(scores[aindex][sindex]) << ",";
        }
      }

      outfile_row = outfile_row_ss.str();
      outfile_row.pop_back();
      std::cout << outfile_row << std::endl;
      outfile << outfile_row << std::endl;
    }
  }

  outfile.close();
}
