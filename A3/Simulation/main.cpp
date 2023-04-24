#include "../Common/AlgorithmRegistrar.h"
#include <dlfcn.h>
#include <iostream>

#include "include/Config.h"
#include "include/InputParser.h"
#include "include/Simulator.h"

#include "pthread.h"
#include <chrono>
#include <cstring>
#include <mutex>
#include <queue>
#include <sched.h>
#include <set>
#include <thread>

using AlgorithmPtr = std::unique_ptr<AlgorithmRegistrar>;
struct RunnableParams;

CmdArgs cmd_args_;
std::mutex mtx, running_params_mtx;

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
  int af_index;
  int h_index;
  int a_index;
  int t_id;

  std::chrono::time_point<std::chrono::system_clock> start_ts;
  size_t timeout = 1000;
  long kill_score = 0;
};

bool operator<(const RunnableParams &lhs, const RunnableParams &rhs) {
  return lhs.t_id < rhs.t_id;
}

void generateSummary(std::vector<SimParams> &sims,
                     std::vector<AlgoParams> &algos,
                     std::vector<std::vector<long>> &scores);

bool loadTestHouseFiles(std::vector<SimParams> &simulators,
                        std::vector<std::string> house_files);

bool loadTestAlgoFiles(std::vector<AlgoParams> &algorithms,
                       std::vector<std::string> algo_files);

void worker(int t_id, std::vector<std::vector<long>> &scores,
            std::queue<RunnableParams> &runnable_params,
            std::set<RunnableParams> &running_params,
            std::vector<Simulator> &simulators) {
  // std::cout << "Thread ID: " << std::this_thread::get_id() << " " << t_id <<
  // " "
  //           << runnable_params.size() << " \nnum_threads "
  //           << cmd_args_.num_threads << std::endl;
  int idx = t_id;
  auto get_fname = [=](auto hname, auto aname) {
    return getStem(hname) + "-" + getStem(aname) + ".txt";
  };
  while (!runnable_params.empty()) {
    RunnableParams param;
    if (runnable_params.empty()) {
      return;
    }
    mtx.lock();
    param = runnable_params.front();
    runnable_params.pop();
    mtx.unlock();

    Simulator &sim = simulators[t_id];
    sim.setDebugFileName(get_fname(param.h_fname, param.a_fname));
    sim.readHouseFile(param.h_fname);
    auto algo =
        AlgorithmRegistrar::getAlgorithmRegistrar().begin() + param.af_index;
    auto algorithm = algo->create();
    sim.setAlgorithm(*algorithm);
    std::cout << get_fname(param.h_fname, param.a_fname) << std::endl;

    param.t_id = t_id;
    param.start_ts = std::chrono::system_clock::now();
    param.kill_score =
        sim.getMaxSteps() * 2 + sim.getInitialDirt() * 300 + 2000;
    param.timeout = 5 * sim.getMaxSteps();
    running_params_mtx.lock();
    running_params.insert(param);
    running_params_mtx.unlock();

    sim.run();
    if (!cmd_args_.summary_only)
      sim.dump(get_fname(param.h_fname, param.a_fname));
    scores[param.a_index][param.h_index] = sim.getScore();

    std::cout << "\tFinished pair: "
              << (getStem(param.a_fname) + '-' + getStem(param.h_fname))
              << std::endl;
    running_params_mtx.lock();
    running_params.erase(param);
    running_params_mtx.unlock();
  }
}

void monitor(std::vector<std::vector<long>> &scores,
             std::queue<RunnableParams> &runnable_params,
             std::set<RunnableParams> &running_params,
             std::vector<std::unique_ptr<std::thread>> &runnable_threads,
             std::vector<Simulator> &simulators) {
  auto get_fname = [=](auto hname, auto aname) {
    return getStem(hname) + "-" + getStem(aname) + ".txt";
  };
  while (1) {
    // monitor thread should sleep for most of the time
    std::this_thread::sleep_for(std::chrono::milliseconds(
        500)); // TODO: maximum of all house max_steps_ * 1ms
    std::set<RunnableParams> to_terminate_params;
    running_params_mtx.lock();
    std::set<RunnableParams> temp_running_params(running_params.begin(),
                                                 running_params.end());
    running_params_mtx.unlock();
    for (auto &running_param : temp_running_params) {
      if (running_param.start_ts +
              std::chrono::milliseconds(running_param.timeout) >
          std::chrono::system_clock::now()) {

        // TODO: check if there should be any output file if thread killed
        if (!cmd_args_.summary_only)
          simulators[running_param.t_id].dump(
              get_fname(running_param.h_fname, running_param.a_fname), true);
        scores[running_param.a_index][running_param.h_index] =
            running_param.kill_score; // MaxSteps * 2 + InitialDirt * 300 + 2000

        std::cout << "\tKilled thread: "
                  << getStem(running_param.a_fname) + '-' +
                         getStem(running_param.h_fname)
                  << std::endl;
        to_terminate_params.insert(running_param);
      }
    }
    for (auto &to_terminate_param : to_terminate_params) {

      std::unique_ptr<std::thread> hogging_thread(
          std::move(runnable_threads[to_terminate_param.t_id]));

#ifdef _POSIX_VERSION
      struct sched_param params;
      params.sched_priority = 0;
      pid_t pthread_id;
      auto thread_id = hogging_thread->get_id();
      std::memcpy(&pthread_id, &thread_id, sizeof(pthread_id));
      sched_setscheduler(pthread_id, SCHED_IDLE, &params);
#endif

      runnable_threads[to_terminate_param.t_id] = std::make_unique<std::thread>(
          worker, to_terminate_param.t_id, std::ref(scores),
          std::ref(runnable_params), std::ref(running_params),
          std::ref(simulators));
      runnable_threads[to_terminate_param.t_id]->detach();
      running_params_mtx.lock();
      running_params.erase(to_terminate_param);
      running_params_mtx.unlock();
    }
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
  std::vector<SimParams> simulator_params(house_files.size());
  /* Init Algo struct*/
  std::vector<AlgoParams> algorithms(algo_files.size());

  std::vector<std::vector<long>> scores(
      algorithms.size(), std::vector<long>(simulator_params.size()));

  bool valid_simulator_found = false, valid_algo_found = true;

  valid_simulator_found = loadTestHouseFiles(simulator_params, house_files);

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

  std::queue<RunnableParams> runnable_params;
  std::set<RunnableParams> running_params;

  for (int aindex = 0; aindex < algorithms.size(); aindex++) {
    if (algorithms[aindex].is_algo_valid) {
      for (int hindex = 0; hindex < simulator_params.size(); hindex++) {
        if (simulator_params[hindex].is_valid) {
          RunnableParams param;
          param.h_fname = simulator_params[hindex].file_name;
          param.a_fname = algorithms[aindex].file_name;
          param.af_index = algorithms[aindex].factory_idx;
          param.a_index = aindex;
          param.h_index = hindex;
          runnable_params.push(std::move(param));

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
  std::vector<Simulator> simulators(cmd_args_.num_threads);
  std::vector<std::unique_ptr<std::thread>> runnable_threads(
      cmd_args_.num_threads);

  std::thread monitor_thread =
      std::thread(monitor, std::ref(scores), std::ref(runnable_params),
                  std::ref(running_params), std::ref(runnable_threads),
                  std::ref(simulators));

  for (int index = 0; index < cmd_args_.num_threads; index++) {
    runnable_threads[index] = std::make_unique<std::thread>(
        worker, index, std::ref(scores), std::ref(runnable_params),
        std::ref(running_params), std::ref(simulators));
  }
  for (int index = 0; index < cmd_args_.num_threads; index++) {
    runnable_threads[index]->detach();
  }
  monitor_thread.detach();

  while (1) {
    std::this_thread::sleep_for(std::chrono::seconds(
        1)); // TODO: replace 1s with maxtimeout of available houses
    if (runnable_params.empty() && running_params.empty()) {
      break;
    }
  }

  generateSummary(simulator_params, algorithms, scores);

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
    Simulator sim;
    auto err = sim.readHouseFile(house_files[index]);
    if ((int)err < 0) {
      std::cerr << err << " at House File:" << getStem(house_files[index])
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
      out_error << "File read error " << getStem(simulators[index].file_name)
                << ". Skipping house file" << std::endl;
      out_error.close();
      continue;
    }

    simulators[index].is_valid = true;
    found = true;
    std::cout << "Simulator " << getStem(simulators[index].file_name)
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
                << getStem(algorithms[index].file_name) << std::endl;
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
    std::cout << "Algorithm " << getStem(algorithms[index].file_name)
              << " loaded successfully " << std::endl;
  }
  return found;
}

void generateSummary(std::vector<SimParams> &sims,
                     std::vector<AlgoParams> &algos,
                     std::vector<std::vector<long>> &scores) {
  // auto &outfile = std::cout;

  std::cout << "Generating summary.csv:" << std::endl;

  std::ofstream outfile("summary.csv");
  if (!outfile.is_open()) {
    std::cerr << "SummaryFileOpenError: Unable to create output file."
              << std::endl
              << "Stopping summary generation.";
    return;
  }

  std::stringstream outfile_row_ss;
  std::string outfile_row;

  outfile_row_ss << "Algo \\ House,";
  for (auto &sim : sims) {
    if (sim.is_valid)
      outfile_row_ss << getStem(sim.file_name) << ",";
    // std::cout << getStem(sim.file_name) << ",";
  }

  outfile_row = outfile_row_ss.str();
  outfile_row.pop_back();
  // std::cout << outfile_row << std::endl;
  outfile << outfile_row << std::endl;

  for (auto aindex = 0; aindex < algos.size(); aindex++) {
    if (algos[aindex].is_algo_valid) {

      outfile_row_ss.str(std::string()); // empty the string stream
      outfile_row_ss << getStem(algos[aindex].file_name) << ",";

      for (auto sindex = 0; sindex < sims.size(); sindex++) {
        if (sims[sindex].is_valid) {
          outfile_row_ss << std::to_string(scores[aindex][sindex]) << ",";
        }
      }

      outfile_row = outfile_row_ss.str();
      outfile_row.pop_back();
      // std::cout << outfile_row << std::endl;
      outfile << outfile_row << std::endl;
    }
  }

  outfile.close();

  std::cout << "Generating summary.csv: DONE!" << std::endl;
}
