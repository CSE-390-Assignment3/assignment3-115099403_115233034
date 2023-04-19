#include "../include/InputParser.h"
#include "../include/ErrorCodes.h"
#include "../include/Utils.h"
#include <fstream>

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
  if (input.empty())
    return (size_t)FileReadError::Invalid;

  lrtrim(input);

  try {
    return std::stoi(input);
  } catch (...) {
    return (size_t)FileReadError::InvalidValue;
  }
}

long readAEqb(std::string input, std::string varname) {
  int idx = input.find('=');
  if (idx == std::string::npos)
    return (long)FileReadError::Invalid;
  auto varstring = input.substr(0, idx);

  lrtrim(varstring);
  if (varstring != varname)
    return (long)FileReadError::Invalid;

  std::string valstring = input.substr(idx + 1);
  return parseInt(valstring);
}

FileReadError populateInput(House &house, RobotState &robot_state,
                            size_t &max_steps,
                            const std::string &input_filename) {
  // populate house_ structure here

  std::string house_name, max_steps_s, max_battery_s, num_rows_s, num_cols_s;
  std::ifstream myfile;
  myfile.open(input_filename);

  if (!myfile.is_open()) {
    return FileReadError::InvalidInputFile;
  }

  std::getline(myfile, house_name);

  std::getline(myfile, max_steps_s);
  max_steps = readAEqb(max_steps_s, "MaxSteps");
  if (max_steps < 0) {
    return FileReadError::InvalidFieldMaxSteps;
  }
  if (myfile.eof()) {
    return FileReadError::Invalid;
  }

  std::getline(myfile, max_battery_s);
  int max_robot_battery_ = readAEqb(max_battery_s, "MaxBattery");
  if (max_robot_battery_ < 0) {
    return FileReadError::InvalidFieldMaxBattery;
  }
  if (myfile.eof()) {
    return FileReadError::Invalid;
  }
  std::getline(myfile, num_rows_s);
  int n_rows_ = readAEqb(num_rows_s, "Rows");
  if (n_rows_ < 0) {
    return FileReadError::InvalidFieldRows;
  }
  if (myfile.eof()) {
    return FileReadError::Invalid;
  }

  std::getline(myfile, num_cols_s);
  int n_cols_ = readAEqb(num_cols_s, "Cols");
  if (n_cols_ < 0) {
    return FileReadError::InvalidFieldCols;
  }

  // std::cout << max_steps_s << std::endl
  //           << max_battery_s << std::endl
  //           << num_rows_s << std::endl
  //           << num_cols_s << std::endl;

  if (myfile.eof()) {
    return FileReadError::Invalid;
  }

  // std::cout << max_steps_ << std::endl
  //           << max_robot_battery_ << std::endl
  //           << n_rows_ << std::endl
  //           << n_cols_ << std::endl;

  std::vector<std::vector<int>> data(n_rows_, std::vector<int>(n_cols_, 0));

  int row_number = 0, col_number = 0, dock_found = 0;
  std::string line;
  while (!myfile.eof()) {
    std::getline(myfile, line);
    for (col_number = 0; col_number < line.size(); col_number++) {
      if (col_number >= n_cols_) {
        break;
      }
      if (std::isdigit(line[col_number])) {
        data[row_number][col_number] = line[col_number] - '0';
      } else if (line[col_number] == ' ') {
        data[row_number][col_number] = 0;
      } else if (line[col_number] == 'W') {
        data[row_number][col_number] = -1; // replace with LocType
      } else if (line[col_number] == 'D' && !dock_found) {
        dock_found = 1;
        data[row_number][col_number] = 100; // replace with LocType
      } else if (line[col_number] == 'D') {
        return FileReadError::InvalidFormatMultipleDocks;
      } else {
        return FileReadError::Invalid;
      }
    }
    row_number++;
    if (row_number == n_rows_)
      break;
  }
  if (!dock_found) {
    std::cout << "ERROR!! Invalid House file no dock found!!" << std::endl;
    return FileReadError::InvalidFormatMissingDock;
  }
  myfile.close();

  house.init(data);
  robot_state.init(max_robot_battery_, house.getDockPos());
  std::cout << "Robot: max_robot_battery:" << max_robot_battery_ << std::endl;

  return FileReadError::None;
}
