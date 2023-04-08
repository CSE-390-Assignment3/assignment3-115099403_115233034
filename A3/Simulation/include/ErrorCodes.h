#pragma once

enum class FileReadError {
  // TODO: Add more codes as we go
  Invalid = -100, // Generic error
  InvalidName,    // A=b incorrect variable name (A)
  InvalidValue,   // A=b incorrect value
  InvalidFormat   // File format incorrect/missing fields/lines
};
