#include "../include/Simulator.h"
#include "../include/ErrorCodes.h"
#include "../include/InputParser.h"
#include "../include/Utils.h"

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

FileReadError Simulator::readHouseFile(const std::string &houseFilePath) {
  return populateInput(house_, robot_state_, max_steps_, houseFilePath);
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
    // std::cout << __PRETTY_FUNCTION__ << " before nextStep" << std::endl;
    Step currentStep = algo->nextStep();
    // std::cout << "Simulator::step Next step " << std::endl;
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
  // std::cout << "After simulation " << house_;
}
void Simulator::dump(std::string output_file_name) {
  std::ofstream outfile(output_file_name);
  if (!outfile) {
    std::cout << "Error opening file " << output_file_name << std::endl;
    std::cout << "Simulator returning without writing to file " << std::endl;
    return;
  }
  outfile << "NumSteps = " << steps_ << std::endl;
  outfile << "DirtLeft = " << house_.totDirt() << std::endl;
  outfile << "Status = " << final_state_ << std::endl;
  outfile << "InDock = "
          << ((robot_state_.getPosition() == house_.getDockPos()) ? "TRUE"
                                                                  : "FALSE")
          << std::endl;
  // TODO: replace 0 with score function
  outfile << "Score = " << 0 << std::endl;

  for (auto step : step_list_)
    outfile << step;
  outfile << std::endl;
  outfile.close();
}
