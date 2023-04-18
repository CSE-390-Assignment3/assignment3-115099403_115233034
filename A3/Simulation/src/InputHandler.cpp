#include "../include/InputHandler.h"
#include "../include/ErrorCodes.h"
#include "../include/InputParser.h"
#include "../include/Utils.h"

int InputHandler::populateInput(House &house, RobotState &robot_state,
                                size_t &max_steps,
                                const std::string &input_filename) {
  // populate house_ structure here

  std::string house_name, max_steps_s, max_battery_s, num_rows_s, num_cols_s;
  std::ifstream myfile;
  myfile.open(input_filename);

  if (!myfile.is_open()) {
    return -1;
  }
  auto print_read_error = [=](auto e) {
    std::cout << "ERROR!! While reading input file : " << FileReadError(e)
              << std::endl;
  };
  std::getline(myfile, house_name);

  std::getline(myfile, max_steps_s);
  max_steps = readAEqb(max_steps_s, "MaxSteps");
  if (max_steps < 0) {
    print_read_error(max_steps);
    return max_steps;
  }
  if (myfile.eof()) {
    return -1;
  }
  std::getline(myfile, max_battery_s);
  int max_robot_battery_ = readAEqb(max_battery_s, "MaxBattery");
  if (max_robot_battery_ < 0) {
    print_read_error(max_robot_battery_);
    return max_robot_battery_;
  }
  if (myfile.eof()) {
    return -1;
  }
  std::getline(myfile, num_rows_s);
  int n_rows_ = readAEqb(num_rows_s, "Rows");
  if (n_rows_ < 0) {
    print_read_error(n_rows_);
    return n_rows_;
  }

  if (myfile.eof()) {
    return -1;
  }
  std::getline(myfile, num_cols_s);
  int n_cols_ = readAEqb(num_cols_s, "Cols");
  if (n_cols_ < 0) {
    print_read_error(n_cols_);
    return n_cols_;
  }

  // std::cout << max_steps_s << std::endl
  //           << max_battery_s << std::endl
  //           << num_rows_s << std::endl
  //           << num_cols_s << std::endl;

  if (myfile.eof()) {
    return -1;
  }

  // std::cout << max_steps_ << std::endl
  //           << max_robot_battery_ << std::endl
  //           << n_rows_ << std::endl
  //           << n_cols_ << std::endl;

  std::vector<std::vector<int>> data(n_rows_, std::vector<int>(n_cols_, 0));

  int row_number = 0, col_number = 0, dock_found = 0;
  std::string line;
  std::getline(myfile, line);
  while (!myfile.eof()) {
    for (col_number = 0; col_number < line.size(); col_number++) {
      if (col_number >= n_cols_)
        break;
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
        std::cout << "ERROR!! Invalid House file More than one dock found!!"
                  << std::endl;
        return -1;
      } else {
        std::cout << "ERROR!! Invalid House data" << std::endl;
        return -1;
      }
    }
    std::getline(myfile, line);
    row_number++;
    if (row_number == n_rows_)
      break;
  }
  if (!dock_found) {
    std::cout << "ERROR!! Invalid House file no dock found!!" << std::endl;
  }
  myfile.close();
  house.init(data);
  robot_state.init(max_robot_battery_, house.getDockPos());
  std::cout << "Robot: max_robot_battery:" << max_robot_battery_ << std::endl;
  // std::cout << house_;

  return 1;
}
