#pragma once

#include "../include/ErrorCodes.h"

#include <string>
#include <vector>

/**
 * All values in lines 2-5 may or may have spaces around the = sign
 * If the file is invalid you can reject it and print the reason to screen
 */
double parseInt(std::string input);

size_t readAEqb(std::string input, std::string varname);

ArgumentsError processArguments(int argc, char **argv, std::string &house,
                                std::string &algo);

std::vector<std::string>
parseDirectory(std::string dirpath, std::string extension, bool trynext = true);

std::string getStem(std::string file_name);
