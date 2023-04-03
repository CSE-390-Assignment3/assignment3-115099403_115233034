//
// Created by Anshuman Funkwal on 3/13/23.
//

#pragma once

#include "../Common/AbstractAlgorithm.h"

class MyAlgorithm: public AbstractAlgorithm{
    std::size_t maxSteps;
    const WallsSensor *WSensor = nullptr;
    const DirtSensor *DSensor = nullptr;
    const BatteryMeter *BMeter = nullptr;
public:

    void setMaxSteps(std::size_t maxSteps) override {
        this->maxSteps = maxSteps;
    }
    void setWallsSensor(const WallsSensor& wallsSensor) override {
        this->WSensor = &wallsSensor;
    }
    void setDirtSensor(const DirtSensor& dirtSensor)  override {
        this->DSensor = &dirtSensor;
    }
    void setBatteryMeter(const BatteryMeter& batteryMeter) override {
        this->BMeter = &batteryMeter;
    }
    // TODO : Complete in cpp file
    Step nextStep() override;
};
