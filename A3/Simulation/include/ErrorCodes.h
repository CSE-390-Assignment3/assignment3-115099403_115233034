#pragma once

enum class FileReadError {
  // TODO: Add more codes as we go
  None = -100,  // Generic error
  Invalid,      // Generic error
  InvalidName,  // A=b incorrect variable name (A)
  InvalidValue, // A=b incorrect value
  InvalidFormat // File format incorrect/missing fields/lines
};

enum class ArgumentsError {
  // Note: add as per need
  None = -100,
  Incomplete,
  MissingHouse,
  MissingAlgo,
};
