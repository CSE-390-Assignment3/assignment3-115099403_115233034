//
// Created by Anshuman Funkwal on 3/13/23.
//

#pragma once

#include "../../Common/AbstractAlgorithm.h"
#include "BatteryMeterImpl.h"
#include "DirtSensorImpl.h"
#include "RobotState.h"
#include "Utils.h"
#include "WallsSensorImpl.h"

#include <fstream>
#include <iostream>
#include <sstream>

class Simulator {
private:
  std::size_t max_steps_;
  std::size_t steps_ = 0;
  std::string final_state_ = "";
  double initial_dirt_;

  House house_;
  RobotState robot_state_;

  std::vector<char> step_list_;
  AbstractAlgorithm *algo;
  // sensors
  DirtSensorImpl dirt_sensor_;
  WallsSensorImpl wall_sensor_;
  BatteryMeterImpl battery_meter_;

  long score_ = -1;
  std::ofstream debug_ostream;
  std::string debug_file_name_;

  int initSensors();

public:
  Simulator();
  ~Simulator() {
    if (debug_ostream.is_open())
      debug_ostream.close();
  };
  FileReadError readHouseFile(const std::string &house_file_path);
  void setAlgorithm(AbstractAlgorithm &algorithm);
  void setDebugFileName(std::string debug_file_name);
  void run();
  void dump(std::string out_file_name, bool is_killed = false);
  long getScore(bool is_killed = false);
  inline bool isRobotInDock() const {
    return robot_state_.getPosition() == house_.getDockPos();
  }
  size_t getMaxSteps() const { return max_steps_; }
  double getInitialDirt() const { return initial_dirt_; }
};
