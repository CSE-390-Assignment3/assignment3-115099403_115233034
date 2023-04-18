#include "../include/InputParser.h"

#include <filesystem>
#include <iostream>

inline void lrtrim(std::string &input) {
  // ltrim
  input.erase(input.begin(),
              std::find_if(input.begin(), input.end(),
                           [](unsigned char ch) { return !std::isspace(ch); }));
  // rtrim
  input.erase(std::find_if(input.rbegin(), input.rend(),
                           [](unsigned char ch) { return !std::isspace(ch); })
                  .base(),
              input.end());
}

double parseInt(std::string input) {
  // std::string input = input.substr(input.find('=') + 1);
  if (input.empty())
    return (size_t)FileReadError::Invalid;

  lrtrim(input);

  try {
    return std::stoi(input);
  } catch (...) {
    return (size_t)FileReadError::InvalidValue;
  }
}

size_t readAEqb(std::string input, std::string varname) {
  int idx = input.find('=');
  if (idx == std::string::npos)
    return (size_t)FileReadError::Invalid;
  auto varstring = input.substr(0, idx);

  lrtrim(varstring);
  if (varstring != varname)
    return (size_t)FileReadError::InvalidName;

  std::string valstring = input.substr(idx + 1);
  return parseInt(valstring);
}

/**
 * @brief Parse directory for extension files and if not found traverses the
 * current working directory
 *
 * @param dirpath
 * @param extension
 * @param trynext
 * @return std::vector<std::string>
 */
std::vector<std::string> parseDirectory(std::string dirpath,
                                        std::string extension, bool trynext) {
  std::vector<std::string> files;
  if (!dirpath.empty()) {
    std::filesystem::path fpath{dirpath};
    if (!std::filesystem::is_directory(fpath)) {
      std::cout << fpath << " is not a directory" << std::endl;
      return {};
    }
    for (auto const &dir_entry : std::filesystem::directory_iterator{fpath}) {
      if (!dir_entry.is_directory() && dir_entry.path().has_extension() &&
          dir_entry.path().extension() == extension) {
        files.push_back(dir_entry.path());
      }
    }
    if (!files.empty())
      return files;
  }
  if (trynext) {
    std::cout << "no matching files in " << dirpath << " with extension "
              << extension << " exploring current directory" << std::endl;
    files = parseDirectory(std::filesystem::current_path(), extension, false);
  }
  return files;
}

ArgumentsError processArguments(int argc, char **argv, std::string &out_house,
                                std::string &out_algo) {
  if (argc < 3) {
    std::cerr << "ArgumentsParsing Error Not enough number of arguments "
              << std::endl;
    return ArgumentsError::Incomplete;
  }

  bool house_path_found = false, algo_path_found = false;

  std::string args_list[] = {"-house_path=", "-algo_path="};

  for (int i = 1; i < argc; i++) {
    std::string arg(argv[i]);
    if (arg.substr(0, 12) == args_list[0]) {
      // house path
      house_path_found = true;
      out_house = arg.substr(12);
    }
    if (arg.substr(0, 11) == args_list[1]) {
      // algo path
      algo_path_found = true;
      out_algo = arg.substr(11);
    }
  }

  if (!house_path_found) {
    std::cerr << "ArgumentsParsing Error House path not found " << std::endl;
    return ArgumentsError::MissingHouse;
  }
  if (!algo_path_found) {
    std::cerr << "ArgumentsParsing Error Algo path not found " << std::endl;
    return ArgumentsError::MissingAlgo;
  }
  return ArgumentsError::None;
}

std::string getStem(std::string file_name) {
  return std::filesystem::path(file_name).stem().string();
}
