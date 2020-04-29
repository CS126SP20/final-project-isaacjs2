// Copyright (c) 2020 [Your Name]. All rights reserved.

#include <sudoku/engine.h>

#include <fstream>
#include <nlohmann/json.hpp>
#include <utility>

using nlohmann::json;

namespace sudoku {
Engine::Engine() {}

Engine::Engine(std::string path) {
  board_path_ = std::move(path);

  ImportGameBoard();

  // Start with no numbers penciled in
  for (auto row : pencil_marks_) {
    for (auto col : row) {
      col.fill(false);
    }
  }
}

void Engine::CreateGame(std::string path) {
  board_path_ = R"(C:\Users\isaac\CLionProjects\Cinder\my-projects\final-project-isaacjs2\resources\easy_1.json)";

  ImportGameBoard();

  // Start with no numbers penciled in
  for (size_t row = 0; row < kBoardSize; row++) {
    for (size_t col = 0; col < kBoardSize; col++) {
      for (size_t num = 0; num < kBoardSize; num++) {
        pencil_marks_[row][col][num] = false;
      }
    }
  }
}

void Engine::ImportGameBoard() {
  std::ifstream infile;
  infile.open(board_path_);

  // Load the file data into a JSON object
  nlohmann::json board_data;
  infile >> board_data;

  board_data.at("board").get_to(current_entries_);
  board_data.at("solution").get_to(solution_);
}

int Engine::GetEntry(int row, int col) const {
  return current_entries_[row][col];
}

void Engine::SetEntry(int row, int col, int num) {
  current_entries_[row][col] = num;
}

bool Engine::IsPenciled(int row, int col, int num) const {
  return pencil_marks_[row][col][num - 1];
}

void Engine::Pencil(int row, int col, int num) {
  pencil_marks_[row][col][num - 1] = true;
}

void Engine::ClearPencilMarks(int row, int col) {
  for (size_t num = 0; num < kBoardSize; num++) {
    pencil_marks_[row][col][num] = false;
  }
}
}  // namespace sudoku
