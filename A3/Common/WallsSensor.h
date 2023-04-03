// DO NOT CHANGE THIS FILE
// Created by Anshuman Funkwal on 3/13/23.
//

#pragma once

#include "common_types.h"

class WallsSensor {
public:
    virtual ~WallsSensor() {}
    virtual bool isWall(Direction d) const = 0;
};
