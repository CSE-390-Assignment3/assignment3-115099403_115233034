#include "../include/Utils.h"
#include "../include/ErrorCodes.h"

using std::string;

inline void lrtrim(string &input) {
  // ltrim
  input.erase(input.begin(),
              std::find_if(input.begin(), input.end(),
                           [](unsigned char ch) { return !std::isspace(ch); }));
  // rtrim
  input.erase(std::find_if(input.rbegin(), input.rend(),
                           [](unsigned char ch) { return !std::isspace(ch); })
                  .base(),
              input.end());
}

double Utils::parseInt(string input) {
  // std::string input = input.substr(input.find('=') + 1);
  if (input.empty())
    return (size_t)FileReadError::Invalid;

  lrtrim(input);

  try {
    return std::stoi(input);
  } catch (...) {
    return (size_t)FileReadError::InvalidValue;
  }
}

size_t Utils::readAEqb(string input, string varname) {
  int idx = input.find('=');
  if (idx == std::string::npos)
    return (size_t)FileReadError::Invalid;
  auto varstring = input.substr(0, idx);

  lrtrim(varstring);
  if (varstring != varname)
    return (size_t)FileReadError::InvalidName;

  string valstring = input.substr(idx + 1);
  return parseInt(valstring);
}

std::ostream &operator<<(std::ostream &out, const Position &pos) {
  out << "(" << pos.r << "," << pos.c << ")";
  return out;
}

Direction reverse(const Direction &direction) {
  return reverseDirections[static_cast<int>(direction)];
}

Step reverse(const Step &step) { return reverseSteps[static_cast<int>(step)]; }

std::ostream &operator<<(std::ostream &out, const Step &step) {
  out << str(step);
  return out;
}

std::ostream &operator<<(std::ostream &out, const FileReadError &error) {
  switch (error) {
  case FileReadError::Invalid:
    out << "FileReadError::Invalid";
    break;
  case FileReadError::InvalidName:
    out << "FileReadError::InvalidName";
    break;
  case FileReadError::InvalidValue:
    out << "FileReadError::InvalidValue";
    break;
  default:
    break;
  }
  return out;
}

std::string str(const Step &step) {
  static std::string string_step[] = {"North", "South", "East",
                                      "West",  "stay",  "Finish"};
  return string_step[static_cast<int>(step)];
}
