#include "../include/InputParser.h"
#include "../include/ErrorCodes.h"

inline void lrtrim(std::string &input) {
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

double parseInt(std::string input) {
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

size_t readAEqb(std::string input, std::string varname) {
  int idx = input.find('=');
  if (idx == std::string::npos)
    return (size_t)FileReadError::Invalid;
  auto varstring = input.substr(0, idx);

  lrtrim(varstring);
  if (varstring != varname)
    return (size_t)FileReadError::InvalidName;

  std::string valstring = input.substr(idx + 1);
  return parseInt(valstring);
}
