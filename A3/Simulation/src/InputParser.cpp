#include "../include/InputParser.h"
#include "../include/ErrorCodes.h"
#include "../include/Utils.h"
#include <filesystem>
#include <fstream>
#include <iostream>

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
  std::cout << house;
  return FileReadError::None;
}

/**
 * @brief Parse directory for extension files and if not found traverses the
 * current working directory
 *
 * @param dirpath
 * @param extension
 * @param trynext
 * @return std::vector<std::string>
 */
std::vector<std::string> parseDirectory(std::string dirpath,
                                        std::string extension, bool trynext) {
  std::vector<std::string> files;
  if (!dirpath.empty()) {
    std::filesystem::path fpath{dirpath};
    if (!std::filesystem::is_directory(fpath)) {
      std::cout << fpath << " is not a directory" << std::endl;
      return {};
    }
    for (auto const &dir_entry : std::filesystem::directory_iterator{fpath}) {
      if (!dir_entry.is_directory() && dir_entry.path().has_extension() &&
          dir_entry.path().extension() == extension) {
        files.push_back(dir_entry.path());
      }
    }
    if (!files.empty())
      return files;
  }
  if (trynext) {
    std::cout << "no matching files in " << dirpath << " with extension "
              << extension << " exploring current directory" << std::endl;
    files = parseDirectory(std::filesystem::current_path(), extension, false);
  }
  return files;
}

ArgumentsError processArguments(int argc, char **argv, std::string &out_house,
                                std::string &out_algo, bool &summary_only) {
  if (argc < 1) {
    std::cerr << "ArgumentsParsing Error Not enough number of arguments "
              << std::endl;
    return ArgumentsError::Incomplete;
  }

  bool house_path_found = false, algo_path_found = false;

  std::string args_list[] = {"-house_path=", "-algo_path=", "-summary_only"};

  for (int i = 1; i < argc; i++) {
    std::string arg(argv[i]);
    if (arg.substr(0, 12) == args_list[0]) {
      // house path
      house_path_found = true;
      out_house = arg.substr(12);
    }
    if (arg.substr(0, 11) == args_list[1]) {
      // algo path
      algo_path_found = true;
      out_algo = arg.substr(11);
    }
    if (arg.substr(0, 13) == args_list[2]) {
      summary_only = true;
    }
  }

  if (!house_path_found) {
    std::cerr << "ArgumentsParsing Error House path not found " << std::endl;
    return ArgumentsError::MissingHouse;
  }
  if (!algo_path_found) {
    std::cerr << "ArgumentsParsing Error Algo path not found " << std::endl;
    return ArgumentsError::MissingAlgo;
  }
  return ArgumentsError::None;
}

std::string getStem(std::string file_name) {
  return std::filesystem::path(file_name).stem().string();
}
