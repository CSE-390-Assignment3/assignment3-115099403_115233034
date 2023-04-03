// DO NOT CHANGE THIS FILE
// Created by Anshuman Funkwal on 3/13/23.
//

#pragma once

#include <cstdlib>

class BatteryMeter {
public:
    virtual ~BatteryMeter() {}
    virtual std::size_t getBatteryState() const = 0;
};
