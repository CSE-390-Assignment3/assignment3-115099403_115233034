#include "Simulator.h"

#include "ErrorCodes.h"
#include "Utils.h"

Simulator::Simulator()
    : robot_state_(), dirt_sensor_(house_, robot_state_),
      wall_sensor_(house_, robot_state_), battery_meter_(robot_state_) {}

void Simulator::setAlgorithm(AbstractAlgorithm &algorithm) {

  algo = &algorithm;
  algo->setMaxSteps(max_steps_);

  algo->setDirtSensor(dirt_sensor_);
  algo->setWallsSensor(wall_sensor_);
  algo->setBatteryMeter(battery_meter_);
}

int Simulator::readHouseFile(const std::string &houseFilePath) {
  // populate house_ structure here

  std::string house_name, max_steps_s, max_battery_s, num_rows_s, num_cols_s;
  std::ifstream myfile;
  myfile.open(houseFilePath);

  if (!myfile.is_open()) {
    return -1;
  }
  auto print_read_error = [=](auto e) {
    std::cout << "ERROR!! While reading input file : " << FileReadError(e)
              << std::endl;
  };
  std::getline(myfile, house_name);

  std::getline(myfile, max_steps_s);
  max_steps_ = Utils::readAEqb(max_steps_s, "MaxSteps");
  if (max_steps_ < 0) {
    print_read_error(max_steps_);
    return max_steps_;
  }
  if (myfile.eof()) {
    return -1;
  }
  std::getline(myfile, max_battery_s);
  int max_robot_battery_ = Utils::readAEqb(max_battery_s, "MaxBattery");
  if (max_robot_battery_ < 0) {
    print_read_error(max_robot_battery_);
    return max_robot_battery_;
  }
  if (myfile.eof()) {
    return -1;
  }
  std::getline(myfile, num_rows_s);
  int n_rows_ = Utils::readAEqb(num_rows_s, "Rows");
  if (n_rows_ < 0) {
    print_read_error(n_rows_);
    return n_rows_;
  }

  if (myfile.eof()) {
    return -1;
  }
  std::getline(myfile, num_cols_s);
  int n_cols_ = Utils::readAEqb(num_cols_s, "Cols");
  if (n_cols_ < 0) {
    print_read_error(n_cols_);
    return n_cols_;
  }

  std::cout << max_steps_s << std::endl
            << max_battery_s << std::endl
            << num_rows_s << std::endl
            << num_cols_s << std::endl;

  if (myfile.eof()) {
    return -1;
  }

  std::cout << max_steps_ << std::endl
            << max_robot_battery_ << std::endl
            << n_rows_ << std::endl
            << n_cols_ << std::endl;

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
  myfile.close();
  house_.init(data);
  robot_state_.init(max_robot_battery_, house_.getDockPos());
  std::cout << "Robot: max_robot_battery:" << max_robot_battery_ << std::endl;
  std::cout << house_;

  return 1;
}

void Simulator::run() {
  bool stop = false, error = true;
  final_state_ = "WORKING";
  while (steps_ <= max_steps_) {
    // std::cout << "Simulator::step " << steps_ << " pos "
    //           << robot_state_.getPosition()
    //           << " Battery: " << battery_meter_.getBatteryState()
    //           << " Dirt: " << dirt_sensor_.dirtLevel() << std::endl;
    error = false;
    Step currentStep = algo->nextStep();
    /** DEAD case handle */
    step_list_.push_back(str(currentStep)[0]);
    if (currentStep == Step::Finish) {
      final_state_ = "FINISHED";
      break;
    } else {
      if (currentStep != Step::Stay &&
          wall_sensor_.isWall(static_cast<Direction>(currentStep))) {
        std::cout << "ERROR!! Running into a wall : unexpected behaviour"
                  << std::endl;
        error = true;
      }
      if (!error) {
        house_.clean(robot_state_.getPosition());
        if (currentStep == Step::Stay &&
            robot_state_.getPosition() == house_.getDockPos()) {
          robot_state_.charge();
        } else {
          robot_state_.step(currentStep);
        }
      }
    }
    if (robot_state_.battery() == 0 &&
        robot_state_.getPosition() != house_.getDockPos()) {
      final_state_ = "DEAD";
      std::cout << "ERROR!! ROBOT REACHED DEAD STATE, STOPPING SIMULATOR"
                << std::endl;
      break;
    }
    steps_++;
    // std::cout << currentStep << " " << house_.totDirt() << std::endl;
  }
  // if (final_state_ == "FINISHED") {
  //   if (robot_state_.battery() == 0 &&
  //       robot_state_.getPosition() != house_.getDockPos())
  //     final_state_ = "DEAD";
  // } else {
  //   final_state_ = "WORKING";
  // }
  std::cout << "After simulation " << house_;
}
void Simulator::dump(std::string outputFileName) {
  std::ofstream myfile;
  myfile.open(outputFileName);
  myfile << "NumSteps = " << steps_ << std::endl;
  myfile << "DirtLeft = " << house_.totDirt() << std::endl;
  myfile << "Status = " << final_state_ << std::endl;
  for (auto step : step_list_)
    myfile << step;
  myfile << std::endl;
  myfile.close();
}
