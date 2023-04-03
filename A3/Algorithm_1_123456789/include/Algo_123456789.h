//
// Created by Anshuman Funkwal on 3/31/23.
//

#ifndef A2_ALGO_123456789_H
#define A2_ALGO_123456789_H

#include <cstddef>
#include "../../Common/AbstractAlgorithm.h"

class Algo_123456789: public AbstractAlgorithm {
public:
    void setMaxSteps(std::size_t maxSteps) override {}
    void setWallsSensor(const WallsSensor&) override {}
    void setDirtSensor(const DirtSensor&) override {}
    void setBatteryMeter(const BatteryMeter&) override {}
    Step nextStep() override {
        return Step::North;
    }
};

#endif //A2_ALGO_123456789_H
