// DO NOT CHANGE THIS FILE
// Created by Anshuman Funkwal on 3/13/23.
//

#pragma once

class DirtSensor {
public:
    virtual ~DirtSensor() {}
    virtual int dirtLevel() const = 0;
};
