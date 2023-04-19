#pragma once

enum class ArgumentsError {
  // Note: add as per need
  None = -100,
  Incomplete,
  MissingHouse,
  MissingAlgo,
};

enum class FileReadError {
  None = 0,
  Invalid = -101,
  InvalidValue,
  InvalidInputFile,
  InvalidFieldName,
  InvalidFieldCols,
  InvalidFieldRows,
  InvalidFieldMaxSteps,
  InvalidFieldMaxBattery,
  InvalidFormatMultipleDocks,
  InvalidFormatMissingDock,
};

std::string string_FileReadError[] = {
    "FileReadError::Invalid",
    "FileReadError::InvalidValue",
    "FileReadError::InvalidInputFile",
    "FileReadError::InvalidFieldName",
    "FileReadError::InvalidFieldCols",
    "FileReadError::InvalidFieldRows",
    "FileReadError::InvalidFieldMaxSteps",
    "FileReadError::InvalidFieldMaxBattery",
    "FileReadError::InvalidFormatMultipleDocks",
    "FileReadError::InvalidFormatMissingDock"};
