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
  std::string file_name_;
  bool dlopen_success = false;
  bool is_algo_valid = false;
  void *handle_ = nullptr;
  std::unique_ptr<AbstractAlgorithm> ptr_ = nullptr;
};

struct SimParams {
  Simulator sim_;
  bool is_valid_ = false;
};

int main(int argc, char **argv) {
  std::string house_path = "", algo_path = "";

  processArguments(argc, argv, house_path, algo_path) != ArgumentsError::None;
  std::cout << "ArgumentsParsing Success!!" << std::endl;

  std::cout << "House path: " << house_path << ", Algo path: " << algo_path
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

  bool valid_simulator_found = false, valid_algo_found = true;

  for (int index = 0; index < house_files.size(); index++) {
    std::stringstream error_buffer;
    if (simulators[index].sim_.readHouseFile(house_files[index], error_buffer) <
        0) {
      std::cout << "Error loading simulator with house " << house_files[index]
                << std::endl;
      std::ofstream out_error;
      out_error.open(getStem(house_files[index]) +
                     std::string(ERROR_EXTENSION_));
      if (!out_error) {
        std::cout << "Error opening error file "
                  << getStem(house_files[index]) + std::string(ERROR_EXTENSION_)
                  << std::endl;
        std::cout << "Skipping writing to error file" << std::endl;
        continue;
      }
      out_error << error_buffer.str() << std::endl;
      out_error << "File read error " << house_files[index]
                << ". Skipping house file" << std::endl;
      out_error.close();
      continue;
    }
    simulators[index].is_valid_ = true;
    valid_simulator_found = true;
    std::cout << "Simulator " << house_files[index] << " loaded successfully "
              << std::endl;
  }

  for (int index = 0, valid_count = 0; index < algo_files.size(); index++) {
    auto registrar_count = AlgorithmRegistrar::getAlgorithmRegistrar().count();
    algorithms[index].file_name_ = algo_files[index];
    algorithms[index].handle_ =
        dlopen(algorithms[index].file_name_.c_str(), RTLD_LAZY);
    if (!algorithms[index].handle_) { // dlopen failed
      std::cout << "error algorithm dlopen " << algorithms[index].file_name_
                << std::endl;
      std::ofstream out_error;
      out_error.open(getStem(algorithms[index].file_name_) +
                     std::string(ERROR_EXTENSION_));
      out_error << "Error loading algorithm library: "
                << algorithms[index].file_name_ << " " << dlerror()
                << std::endl;
      out_error.close();
      continue;
    }
    algorithms[index].dlopen_success = true; // success

    if (AlgorithmRegistrar::getAlgorithmRegistrar().count() ==
        registrar_count) { // algorithm has not been registered
      std::ofstream out_error;
      out_error.open(getStem(algorithms[index].file_name_) +
                     std::string(ERROR_EXTENSION_));
      out_error << "Algorithm registration failed: "
                << algorithms[index].file_name_ << std::endl;
      out_error.close();
    }
    auto algo =
        AlgorithmRegistrar::getAlgorithmRegistrar().begin() + valid_count++;
    try {
      std::cout << "creating algorithm " << index << " "
                << algorithms[index].file_name_ << std::endl;
      algorithms[index].ptr_ = algo->create();
      algorithms[index].is_algo_valid = true;
      valid_algo_found = true;
    } catch (...) {
      std::ofstream out_error;
      out_error.open(getStem(algorithms[index].file_name_) +
                     std::string(ERROR_EXTENSION_));
      out_error << "Algorithm create failed : " << algorithms[index].file_name_
                << std::endl;
      out_error.close();
    }
    std::cout << "Algorithm " << algorithms[index].file_name_
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

  // for (const auto &algo : AlgorithmRegistrar::getAlgorithmRegistrar()) {
  //   auto algorithm = algo.create();
  //   simulator.setAlgorithm(*algorithm);
  //   simulator.run();
  //   simulator.dump("output.txt");
  //   std::cout << algo.name() << ": " <<
  //   static_cast<int>(algorithm->nextStep())
  //             << std::endl;
  // }

  for (int aindex = 0; aindex < algorithms.size(); aindex++) {
    if (algorithms[aindex].is_algo_valid) {
      for (int hindex = 0; hindex < house_files.size(); hindex++) {
        // if ()
      }
    }
  }

  algorithms.clear();
  AlgorithmRegistrar::getAlgorithmRegistrar().clear();

  for (int index = 0; index < algo_files.size(); index++) {
    if (algorithms[index].dlopen_success) {
      dlclose(algorithms[index].handle_);
    }
  }
  return 0;
}
