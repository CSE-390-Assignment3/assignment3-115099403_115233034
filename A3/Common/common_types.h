// DO NOT CHANGE THIS FILE
// Created by Anshuman Funkwal on 3/13/23.
//

#pragma once

enum class Direction { North, East, South, West };
constexpr Direction reverseDirections[] = {Direction::South, Direction::West,
                                           Direction::North, Direction::East};

enum class Step { North, East, South, West, Stay, Finish };
constexpr Step reverseSteps[] = {Step::South, Step::West, Step::North,
                                 Step::East, Step::Stay};

// config
#define MAX_DIRT 9
enum class LocType {
  Wall = -1,
  Dock = 100,
};
