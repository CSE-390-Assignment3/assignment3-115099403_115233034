#pragma once

#include "../../Common/BatteryMeter.h"
#include "../../Common/common_types.h"
#include "House.h"
#include "RobotState.h"

#include <memory>

class BatteryMeterImpl : public BatteryMeter {
private:
  const RobotState *robot_;

public:
  BatteryMeterImpl(const RobotState &robotState);
  std::size_t getBatteryState() const override;
};
