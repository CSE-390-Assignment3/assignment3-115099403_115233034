#include "../include/Simulator.h"
#include "../include/ErrorCodes.h"
#include "../include/InputParser.h"
#include "../include/Utils.h"
#include "../include/config.h"

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
void Simulator::setDebugFileName(std::string debug_file_name) {
  debug_file_name_ =
      (debug_file_name.size() ? debug_file_name : "default") + DEBUG_EXTENSION_;
}

FileReadError Simulator::readHouseFile(const std::string &houseFilePath) {
  if (debug_ostream.is_open())
    debug_ostream.close();
  debug_ostream.open(debug_file_name_);
  house_ = House();
  robot_state_ = RobotState();
  steps_ = 0;
  step_list_.clear();
  auto error = populateInput(house_, robot_state_, max_steps_, houseFilePath);
  initial_dirt_ = house_.totDirt();
  return error;
}

void Simulator::run() {
  bool stop = false, error = true;
  final_state_ = "WORKING";
  debug_ostream << "Before simulation: " << house_;
  while (steps_ <= max_steps_) {
    // debug_ostream << "Simulator::step " << steps_ << " pos "
    //           << robot_state_.getPosition()
    //           << " Battery: " << battery_meter_.getBatteryState()
    //           << " Dirt: " << dirt_sensor_.dirtLevel() << std::endl;
    error = false;
    // debug_ostream << __PRETTY_FUNCTION__ << " before nextStep" << std::endl;
    Step currentStep = algo->nextStep();
    // debug_ostream << "Simulator::step Next step " << std::endl;
    /** DEAD case handle */
    step_list_.push_back(str(currentStep)[0]);
    if (currentStep == Step::Finish) {
      final_state_ = "FINISHED";
      break;
    } else {
      if (currentStep != Step::Stay &&
          wall_sensor_.isWall(static_cast<Direction>(currentStep))) {
        debug_ostream << "ERROR!! Running into a wall : unexpected behaviour"
                      << std::endl;
        error = true;
      }
      if (!error) {
        house_.clean(robot_state_.getPosition());
        if (currentStep == Step::Stay && isRobotInDock()) {
          robot_state_.charge();
        } else {
          robot_state_.step(currentStep);
        }
      }
    }
    if (robot_state_.battery() == 0 && !isRobotInDock()) {
      final_state_ = "DEAD";
      debug_ostream << "ERROR!! ROBOT REACHED DEAD STATE, STOPPING SIMULATOR"
                    << std::endl;
      break;
    }
    steps_++;
    // debug_ostream << currentStep << " " << house_.totDirt() << std::endl;
  }
  debug_ostream << "After simulation: " << house_;
  debug_ostream << final_state_ << std::endl;
}

void Simulator::dump(std::string output_file_name, bool is_killed) {
  std::ofstream outfile(output_file_name);
  if (!outfile) {
    std::cout << "Error opening file " << output_file_name << std::endl;
    std::cout << "Simulator returning without writing to file " << std::endl;
    return;
  }
  outfile << "NumSteps = "
          << (step_list_.empty() ? 0
                                 : (step_list_.back() == str(Step::Finish)[0]
                                        ? (step_list_.size() - 1)
                                        : step_list_.size()))
          << std::endl;
  outfile << "DirtLeft = " << house_.totDirt() << std::endl;
  outfile << "Status = " << (is_killed ? "DEAD" : final_state_) << std::endl;
  outfile << "InDock = " << ((isRobotInDock()) ? "TRUE" : "FALSE") << std::endl;
  outfile << "Score = " << getScore(is_killed) << std::endl;
  if (is_killed)
    outfile << "Killed thread due timeout" << std::endl;
  for (auto step : step_list_)
    outfile << step;
  outfile << std::endl;
  outfile.close();
}

long Simulator::getScore(bool is_killed) {

  // If DEAD =>
  //     Score = MaxSteps + DirtLeft * 300 + 2000
  // Otherwise, If reported FINISHED and robot is NOT InDock  =>
  //     Score = MaxSteps + DirtLeft * 300 + 3000
  // Otherwise =>
  //     Score = NumSteps + DirtLeft * 300 + (InDock? 0 : 1000)
  if (is_killed) {
    score_ = max_steps_ * 2 + initial_dirt_ * 300 + 2000;
  } else if (final_state_ == "DEAD") {
    score_ = max_steps_ + house_.totDirt() * 300 + 2000;
  } else if (final_state_ == "FINISHED" && !isRobotInDock()) {
    score_ = max_steps_ + house_.totDirt() * 300 + 3000;
  } else {
    score_ = steps_ + house_.totDirt() * 300 + (isRobotInDock() ? 0 : 1000);
  }

  return score_;
}
