#include <iostream>
// #include "Simulation/Simulator.h"
// #include "AlgorithmCommon/Algorithm_1.h"
#include "../Common/AlgorithmRegistrar.h"
#include <dlfcn.h>

#include "include/Simulator.h"

using AlgorithmPtr = std::unique_ptr<AlgorithmRegistrar>;

// getting command line arguments for the house file
int main(int argc, char **argv) {

  // TODO: Create the simulator, move the code loading the algorithms into
  // simulator class,
  //       make sure you load all algorithms in the proper folder and not only
  //       two.
  Simulator simulator;
  //  TODO: Handle empty command line args etc.
  if (argc < 2) {
    std::cout << "Error!! Not enough number of arguments " << std::endl;
    return -1;
  }

  if (simulator.readHouseFile(argv[1]) < 0)
    std::cout << "File read error. Stopping Simulator" << std::endl;

  std::cout << "Simulator reading file Done! \n";

  // Load algorithm library
  void *algorithm1_handle = dlopen("Algorithm_1/libAlgorithm_1.so", RTLD_LAZY);
  if (!algorithm1_handle) {
    std::cerr << "Error loading algorithm library: " << dlerror() << std::endl;
    return 1;
  } else {
    std::cout << "Algorithm_1 loaded successfully " << std::endl;
  }

  // void *algorithm_handle1 =
  //     dlopen("Algorithm_1_123456789/libAlgorithm_1_123456789.so", RTLD_LAZY);
  // if (!algorithm_handle1) {
  //   std::cerr << "Error loading algorithm library: " << dlerror() <<
  //   std::endl; return 1;
  // } else {
  //   std::cout << "Algorithm_1_123456789 loaded successfully " << std::endl;
  // }

  // void *algorithm_handle2 =
  //     dlopen("Algorithm_2_123456789/libAlgorithm_2_123456789.so", RTLD_LAZY);
  // if (!algorithm_handle2) {
  //   std::cerr << "Error loading algorithm library: " << dlerror() <<
  //   std::endl; AlgorithmRegistrar::getAlgorithmRegistrar().clear();
  //   dlclose(algorithm_handle1);
  //   return 1;
  // } else {
  //   std::cout << "Algorithm_2_123456789 loaded successfully " << std::endl;
  // }

  std::cout << "AlgorithmRegistrar count "
            << AlgorithmRegistrar::getAlgorithmRegistrar().count() << std::endl;

  for (const auto &algo : AlgorithmRegistrar::getAlgorithmRegistrar()) {
    auto algorithm = algo.create();
    simulator.setAlgorithm(*algorithm);
    simulator.run();
    simulator.dump("output.txt");
    std::cout << algo.name() << ": " << static_cast<int>(algorithm->nextStep())
              << std::endl;
  }

  AlgorithmRegistrar::getAlgorithmRegistrar().clear();
  dlclose(algorithm1_handle);
  // dlclose(algorithm_handle1);
  // dlclose(algorithm_handle2);
}
