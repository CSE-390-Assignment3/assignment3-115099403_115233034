//
// Created by Anshuman Funkwal on 3/30/23.
//

#pragma once

#include "../Common/AlgorithmRegistrar.h"
#include <string>

struct AlgorithmRegistration {
  AlgorithmRegistration(const std::string &name,
                        AlgorithmFactory algorithmFactory) {
    AlgorithmRegistrar::getAlgorithmRegistrar().registerAlgorithm(
        name, std::move(algorithmFactory));
  }
};

#define REGISTER_ALGORITHM(ALGO)                                               \
  AlgorithmRegistration _##ALGO(#ALGO, [] { return std::make_unique<ALGO>(); })
