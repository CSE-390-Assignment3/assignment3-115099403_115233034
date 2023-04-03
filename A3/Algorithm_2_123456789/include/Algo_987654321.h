//
// Created by Anshuman Funkwal on 3/31/23.
//

#ifndef A2_ALGO_987654321_H
#define A2_ALGO_987654321_H

#include <cstddef>
#include "../../Common/AbstractAlgorithm.h"

class Algo_987654321: public AbstractAlgorithm {
public:
    void setMaxSteps(std::size_t maxSteps) override {}
    void setWallsSensor(const WallsSensor&) override {}
    void setDirtSensor(const DirtSensor&) override {}
    void setBatteryMeter(const BatteryMeter&) override {}
    Step nextStep() override {
        return Step::South;
    }
};

#endif //A2_ALGO_987654321_H
