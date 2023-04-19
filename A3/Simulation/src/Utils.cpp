#include "../include/Utils.h"
#include "../include/ErrorCodes.h"

Direction reverse(const Direction &direction) {
  return reverseDirections[static_cast<int>(direction)];
}

Step reverse(const Step &step) { return reverseSteps[static_cast<int>(step)]; }

std::ostream &operator<<(std::ostream &out, const Position &pos) {
  out << "(" << pos.r << "," << pos.c << ")";
  return out;
}

std::ostream &operator<<(std::ostream &out, const Step &step) {
  out << str(step);
  return out;
}

std::ostream &operator<<(std::ostream &out, const FileReadError &error) {
  return out << string_FileReadError[static_cast<int>(error) -
                                     static_cast<int>(FileReadError::Invalid)];
}

std::string str(const Step &step) {
  static std::string string_step[] = {"North", "South", "East",
                                      "West",  "stay",  "Finish"};
  return string_step[static_cast<int>(step)];
}
