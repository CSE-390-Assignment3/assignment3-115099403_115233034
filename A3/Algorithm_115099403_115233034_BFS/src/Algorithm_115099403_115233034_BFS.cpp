#include "../../AlgorithmCommon/AlgorithmRegistration.h"

#include "../include/Algorithm_115099403_115233034_BFS.h"

#include "../include/Utils.h"

#include <queue>

REGISTER_ALGORITHM(Algorithm_115099403_115233034_BFS);

Algorithm_115099403_115233034_BFS::Algorithm_115099403_115233034_BFS()
    : steps_(0), house_manager_(), current_position_(DOCK_POS),
      state_(AlgoState::CHARGING) {
  house_manager_.setDirt(current_position_, int(LocType::Dock));
}

Algorithm_115099403_115233034_BFS::Algorithm_115099403_115233034_BFS(
    AbstractAlgorithm &algorithm) {
  *this = algorithm;
}

void Algorithm_115099403_115233034_BFS::setMaxSteps(std::size_t max_steps) {
  max_steps_ = max_steps;
}

void Algorithm_115099403_115233034_BFS::setWallsSensor(
    const WallsSensor &walls_sensor) {
  walls_sensor_ = &walls_sensor;
}

void Algorithm_115099403_115233034_BFS::setDirtSensor(
    const DirtSensor &dirt_sensor) {
  dirt_sensor_ = &dirt_sensor;
}

void Algorithm_115099403_115233034_BFS::setBatteryMeter(
    const BatteryMeter &battery_meter) {
  battery_meter_ = &battery_meter;
  max_battery_ = battery_meter_->getBatteryState();
}

void Algorithm_115099403_115233034_BFS::updateNeighbors() {
  house_manager_.eraseUnexplored(current_position_);

  for (auto dir : dirPriority()) {
    house_manager_.updateNeighbor(dir, current_position_,
                                  walls_sensor_->isWall(dir));
  }
}

bool Algorithm_115099403_115233034_BFS::needCharge() {
  if (current_position_ == DOCK_POS)
    return false;
  auto st = house_manager_.getShortestPath(current_position_, DOCK_POS);
  // std::cout << __FUNCTION__ << " st.size " << st.size() << std::endl;
  if (st.size() + BATTERY_BUFF > battery_meter_->getBatteryState() ||
      st.size() >= (max_steps_ - steps_))
    return true;
  return false;
}

Step Algorithm_115099403_115233034_BFS::work() {
  // Assuming current_pos exists in percieved
  // priority to cleaning
  // std::cout << __FUNCTION__ << std::endl;
  if (house_manager_.dirt(current_position_) > 0)
    return Step::Stay;

  Direction dir;
  int max_dirt = -1;
  // it is guaranteed to be in perceived_house_ or unexplored_
  // due to updateNeighbors()
  for (auto d : dirPriority()) {
    auto point = getPosition(current_position_, d);
    if (house_manager_.isUnexplored(point)) {
      current_position_ = getPosition(current_position_, d);
      return static_cast<Step>(d);
    } else if (house_manager_.exists(point) && house_manager_.dirt(point) > 0) {
      if (max_dirt < house_manager_.dirt(point)) {
        dir = d;
        max_dirt = house_manager_.dirt(point);
      }
    } else if (!house_manager_.exists(point)) {
      std::cout << "ERROR!! INVALID SCENARIO - NOT IN UNEXPLORED OR PERCIEVED"
                << std::endl;
    }
  }
  if (max_dirt > 0) {
    current_position_ = getPosition(current_position_, dir);
    return static_cast<Step>(dir);
  }
  // BFS ALGORITHM
  state_ = AlgoState::TO_POS;
  // populate stack
  stack_ = house_manager_.getShortestPath(current_position_, {}, true);
  if (stack_.size() * 2 > max_battery_)
    return Step::Finish;
  if (stack_.empty()) {
    return Step::Finish;
  }
  dir = stack_.top();
  stack_.pop();
  current_position_ = getPosition(current_position_, dir);
  return static_cast<Step>(dir);
}

/**
 * @todo
 * 1. handle total dirt
 */
Step Algorithm_115099403_115233034_BFS::nextStep() {
  if (battery_meter_->getBatteryState() == 1 && steps_ == 0 ||
      (max_steps_ - steps_ <= 1 && current_position_ == DOCK_POS)) // DEAD case
    return Step::Finish;

  steps_++;
  // std::cout << __PRETTY_FUNCTION__ << " currentpos: " <<
  // current_position_.first
  //           << " " << current_position_.second
  //           << " totaldirt: " << house_manager_.total_dirt() << " " << state_
  //           << " unexplored " << house_manager_.isUnexploredEmpty()
  //           << std::endl;

  if (steps_ != 1 && house_manager_.isUnexploredEmpty() &&
      house_manager_.total_dirt() == 0 && current_position_ == DOCK_POS)
    state_ = AlgoState::FINISH;

  if (state_ == AlgoState::FINISH) {
    return Step::Finish;
  }

  house_manager_.setDirt(current_position_, dirt_sensor_->dirtLevel());

  updateNeighbors();
  house_manager_.clean(current_position_, dirt_sensor_->dirtLevel());

  if (state_ == AlgoState::CHARGING) {
    // std::cout << __FUNCTION__ << "CHARGING" << std::endl;
    if (battery_meter_->getBatteryState() != max_battery_) {
      return Step::Stay;
    }
    state_ = AlgoState::WORKING;
  }

  if (steps_ != 1 && (needCharge() || (house_manager_.isUnexploredEmpty() &&
                                       house_manager_.total_dirt() == 0))) {
    // std::cout << __FUNCTION__ << "NEED-CHARGE" << std::endl;
    state_ = AlgoState::TO_DOCK;
    // populate stack
    stack_ = house_manager_.getShortestPath(current_position_, DOCK_POS);
    auto dir = stack_.top();
    stack_.pop();
    current_position_ = getPosition(current_position_, dir);
    if (stack_.empty())
      state_ = AlgoState::CHARGING;
    return static_cast<Step>(dir);
  } else if (state_ == AlgoState::TO_DOCK || state_ == AlgoState::TO_POS) {
    // std::cout << __FUNCTION__ << "TO_DOCK/POS" << std::endl;
    // if (state_ == AlgoState::TO_POS && needCharge()) {
    //   stack_ = std::stack<Direction>(); // clear stack
    //   state_ = AlgoState::TO_DOCK;
    // }
    auto dir = stack_.top();
    stack_.pop();
    // @todo check for correctness
    // if position is valid i.e unexplored or perceived
    current_position_ = getPosition(current_position_, dir);
    if (stack_.empty())
      state_ = DOCK_POS == current_position_ ? AlgoState::CHARGING
                                             : AlgoState::WORKING;
    return static_cast<Step>(dir);
  } else {
    return work();
  }
}
