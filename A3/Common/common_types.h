// DO NOT CHANGE THIS FILE
// Created by Anshuman Funkwal on 3/13/23.
//

#pragma once

enum class Direction { North, East, South, West };
enum class Step { North, East, South, West, Stay, Finish };

// config
#define MAX_DIRT 9
enum class LocType {
  Wall = -1,
  Dock = 100,
};
