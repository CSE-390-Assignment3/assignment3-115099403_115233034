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

class Simulator {
private:
  std::size_t max_steps_;
  std::size_t steps_ = 0;
  std::string final_state_ = "";
  House house_;
  RobotState robot_state_;
  std::vector<char> step_list_;
  AbstractAlgorithm *algo;
  // sensors
  DirtSensorImpl dirt_sensor_;
  WallsSensorImpl wall_sensor_;
  BatteryMeterImpl battery_meter_;

  int initSensors();

public:
  Simulator();
  FileReadError readHouseFile(const std::string &house_file_path);
  void setAlgorithm(AbstractAlgorithm &algorithm);
  void run();
  void dump(std::string outFileName);
};
