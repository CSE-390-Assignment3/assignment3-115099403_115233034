#include <iostream>
// #include "Simulation/Simulator.h"
// #include "AlgorithmCommon/Algorithm_1.h"
#include "../Common/AlgorithmRegistrar.h"
#include <dlfcn.h>

#include "include/ErrorCodes.h"
#include "include/Simulator.h"
#include "include/config.h"

#include <filesystem>
#include <string>

using AlgorithmPtr = std::unique_ptr<AlgorithmRegistrar>;

ArgumentsError processArguments(int argc, char **argv, std::string &house,
                                std::string &algo);

std::vector<std::string> parseDirectory(std::string dirpath,
                                        std::string extension,
                                        bool trynext = true) {
  std::vector<std::string> files;
  if (!dirpath.empty()) {
    std::filesystem::path fpath{dirpath};
    if (!std::filesystem::is_directory(fpath)) {
      std::cout << fpath << " is not a directory" << std::endl;
      return {};
    }
    for (auto const &dir_entry : std::filesystem::directory_iterator{fpath}) {
      if (!dir_entry.is_directory() && dir_entry.path().has_extension() &&
          dir_entry.path().extension() == extension) {
        files.push_back(dir_entry.path());
      }
    }
    if (!files.empty())
      return files;
  }
  std::cout << "no matching files, exploring current directory" << std::endl;
  files = parseDirectory(std::filesystem::current_path(), extension, false);
  return files;
}

// getting command line arguments for the house file
int main(int argc, char **argv) {
  //  TODO: Handle empty command line args etc.
  std::string house_path = "", algo_path = "";
  // std::vector<std::string> houses, algos;

  processArguments(argc, argv, house_path, algo_path) != ArgumentsError::None;
  std::cout << "ArgumentsParsing Success!!" << std::endl;

  std::cout << "House path: " << house_path << ", Algo path: " << algo_path
            << std::endl;
  std::cout << "current path: " << std::filesystem::current_path() << std::endl;

  auto algo_files = parseDirectory(algo_path, ALGO_EXTENSION_);
  auto house_files = parseDirectory(house_path, HOUSE_EXTENSION_);

  std::cout << "algo_files: \n";
  for (auto v : algo_files)
    std::cout << v << std::endl;
  std::cout << "house_files: \n";
  for (auto v : house_files)
    std::cout << v << std::endl;

  std::vector<Simulator> simulators(house_files.size());
  std::vector<bool> is_simulator_valid(house_files.size());

  for (int index = 0; index < house_files.size(); index++) {
    if (simulators[index].readHouseFile(house_files[index]) < 0) {
      std::cout << "File read error " << house_files[index]
                << ". Skipping house file" << std::endl;
    }
    is_simulator_valid[index] = true;
  }

  std::vector<void *> algo_handles(algo_files.size());
  std::vector<bool> is_algo_handles_valid(algo_files.size());

  for (int index = 0; index < algo_files.size(); index++) {
    algo_handles[index] = dlopen(algo_files[index].c_str(), RTLD_LAZY);
    if (!algo_handles[index]) {
      std::cerr << "Error loading algorithm library: " << algo_files[index]
                << " " << dlerror() << std::endl;
      continue;
    }
    is_algo_handles_valid[index] = true;
    std::cout << "Algorithm " << algo_files[index] << " loaded successfully "
              << std::endl;
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

  AlgorithmRegistrar::getAlgorithmRegistrar().clear();
  for (int index = 0; index < algo_files.size(); index++) {
    if (is_algo_handles_valid[index])
      dlclose(algo_handles[index]);
  }
}

ArgumentsError processArguments(int argc, char **argv, std::string &out_house,
                                std::string &out_algo) {
  if (argc < 3) {
    std::cerr << "ArgumentsParsing Error Not enough number of arguments "
              << std::endl;
    return ArgumentsError::Incomplete;
  }

  bool house_path_found = false, algo_path_found = false;

  std::string args_list[] = {"-house_path=", "-algo_path="};

  for (int i = 1; i < argc; i++) {
    std::string arg(argv[i]);
    if (arg.substr(0, 12) == args_list[0]) {
      // house path
      house_path_found = true;
      out_house = arg.substr(12);
    }
    if (arg.substr(0, 11) == args_list[1]) {
      // algo path
      algo_path_found = true;
      out_algo = arg.substr(11);
    }
  }

  if (!house_path_found) {
    std::cerr << "ArgumentsParsing Error House path not found " << std::endl;
    return ArgumentsError::MissingHouse;
  }
  if (!algo_path_found) {
    std::cerr << "ArgumentsParsing Error Algo path not found " << std::endl;
    return ArgumentsError::MissingAlgo;
  }
  return ArgumentsError::None;
}
