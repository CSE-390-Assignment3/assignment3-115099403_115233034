#pragma once

#include "../include/ErrorCodes.h"
#include "../include/House.h"
#include "../include/RobotState.h"

#include <string>
#include <vector>

/**
 * All values in lines 2-5 may or may have spaces around the = sign
 * If the file is invalid you can reject it and print the reason to screen
 */
double parseInt(std::string input);

long readAEqb(std::string input, std::string varname);

FileReadError populateInput(House &house, RobotState &robot_state,
                            size_t &max_steps,
                            const std::string &input_filename);

ArgumentsError processArguments(int argc, char **argv, std::string &house,
                                std::string &algo);

std::vector<std::string>
parseDirectory(std::string dirpath, std::string extension, bool trynext = true);

std::string getStem(std::string file_name);
