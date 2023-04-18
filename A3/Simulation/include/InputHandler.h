#pragma once

#include "../../Common/AbstractAlgorithm.h"
#include "House.h"
#include "RobotState.h"
#include "Utils.h"
#include <fstream>
#include <string>

class InputHandler {
public:
  InputHandler() {}
  static int populateInput(House &house, RobotState &robot_state,
                           size_t &max_steps,
                           const std::string &input_filename);
};
