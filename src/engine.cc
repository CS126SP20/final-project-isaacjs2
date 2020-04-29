// Copyright (c) 2020 [Your Name]. All rights reserved.

#include <sudoku/engine.h>

#include <fstream>
#include <nlohmann/json.hpp>

using nlohmann::json;

namespace sudoku {
  Engine::Engine(std::string path) {
    board_path_ = path;

    ImportGameBoard();
  }

  void Engine::ImportGameBoard() {
    std::ifstream infile;
    infile.open(board_path_);

    // Load the file data into a JSON object
    nlohmann::json board_data;
    infile >> board_data;
  }
}  // namespace sudoku
