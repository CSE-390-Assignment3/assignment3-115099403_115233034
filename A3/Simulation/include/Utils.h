#pragma once

#include "../../Common/common_types.h"
#include "ErrorCodes.h"

#define MAX_STEPS_CHARGE 20

#include <iostream>
#include <string>

/**
 * All values in lines 2-5 may or may have spaces around the = sign
 * If the file is invalid you can reject it and print the reason to screen
 */
class Utils {
public:
  static double parseInt(std::string input);
  static size_t readAEqb(std::string input, std::string varname);
};

struct Position {
  int r, c;
  Position next(const Step &d) {
    switch (d) {
    case Step::North:
      return {r - 1, c};
    case Step::South:
      return {r + 1, c};
    case Step::West:
      return {r, c - 1};
    case Step::East:
      return {r, c + 1};
    default:
      return {r, c};
    }
  }
  bool operator==(const Position &p) { return (r == p.r && c == p.c); }
  bool operator!=(const Position &p) { return !(*this == p); }
};

std::ostream &operator<<(std::ostream &out, const Position &pos);
std::ostream &operator<<(std::ostream &out, const Step &step);
std::ostream &operator<<(std::ostream &out, const FileReadError &error);

Direction reverse(const Direction &d);
Step reverse(const Step &s);
std::string str(const Step &step);
