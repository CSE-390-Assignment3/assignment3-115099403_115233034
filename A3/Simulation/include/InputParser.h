#pragma once

#include "../include/House.h"
#include "../include/RobotState.h"
#include <string>

/**
 * All values in lines 2-5 may or may have spaces around the = sign
 * If the file is invalid you can reject it and print the reason to screen
 */
double parseInt(std::string input);

long readAEqb(std::string input, std::string varname);

FileReadError populateInput(House &house, RobotState &robot_state,
                            size_t &max_steps,
                            const std::string &input_filename);
