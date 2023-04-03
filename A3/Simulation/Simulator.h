//
// Created by Anshuman Funkwal on 3/13/23.
//

#pragma once

#include <string>
#include "../Common/AbstractAlgorithm.h"

// You may want to add additional classes for simulation but you should have a Simulator class.
// You can use the below suggestion or alternatively implement concrete class for each sensor.
class Simulator : public WallsSensor, public DirtSensor, public BatteryMeter {
    AbstractAlgorithm *algo;
    std::size_t maxSteps;
public:

    void readHouseFile(const std::string &houseFilePath) {
        // TODO : Consider moving this function to cpp
    }

    void setAlgorithm(AbstractAlgorithm &algorithm) {
        algo = &algorithm;
        algo->setMaxSteps(maxSteps);
        algo->setWallsSensor(*this);
        algo->setDirtSensor(*this);
        algo->setBatteryMeter(*this);
    }

    void run () {
        // TODO : Implement run() using the following function
        algo->nextStep();
    }

    bool isWall(Direction d) const override {
        return false;
    }

    int dirtLevel() const override {
        return 0;
    }

    std::size_t getBatteryState() const override {
        return 0;
    }
};
