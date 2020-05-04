// Copyright (c) 2020 [Your Name]. All rights reserved.

#include <sudoku/engine.h>

#include <fstream>
#include <nlohmann/json.hpp>
#include <utility>

using nlohmann::json;

namespace sudoku {
Engine::Engine() : difficulty_{Difficulty::kEasy},
              is_penciling_{false},
              easy_boards_{"easy_1.json", "easy_2.json", "easy_3.json"},
              medium_boards_{"medium_1.json", "medium_2.json", "medium_3.json"},
              hard_boards_{"hard_1.json", "hard_2.json", "hard_3.json"}
              {}

void Engine::CreateGame(Difficulty difficulty) {
  // Get a board of the right difficulty
  switch (difficulty) {
    case Difficulty::kEasy :
      board_path_ = "C:\\Users\\isaac\\CLionProjects\\Cinder\\my-projects"
                    "\\final-project-isaacjs2\\resources\\easy_1.json";
      break;
    case Difficulty::kMedium :
      board_path_ = "C:\\Users\\isaac\\CLionProjects\\Cinder\\my-projects"
                    "\\final-project-isaacjs2\\resources\\medium_1.json";
      break;
    case Difficulty::kHard :
      board_path_ = "C:\\Users\\isaac\\CLionProjects\\Cinder\\my-projects"
                    "\\final-project-isaacjs2\\resources\\hard_1.json";
      break;
  }

  ImportGameBoard();

  for (size_t row = 0; row < kBoardSize; row++) {
    for (size_t col = 0; col < kBoardSize; col++) {
      if (current_entries_[row][col] != 0) {
        entry_states_[row][col] = EntryState::kCorrect;
      } else {
        entry_states_[row][col] = EntryState::kUnknown;
      }

      // Start with no numbers penciled in
      for (size_t num = 0; num < kBoardSize; num++) {
        pencil_marks_[row][col][num] = false;
      }
    }
  }

  is_penciling_ = false;
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

void Engine::ChangePencilMark(int row, int col, int num) {
  pencil_marks_[row][col][num - 1] = !pencil_marks_[row][col][num - 1];
}

void Engine::ClearPencilMarks(int row, int col) {
  for (size_t num = 0; num < kBoardSize; num++) {
    pencil_marks_[row][col][num] = false;
  }
}
bool Engine::IsPenciling() const {
  return is_penciling_;
}
void Engine::SwitchEntryMode() {
  is_penciling_ = !is_penciling_;
}

Engine::Difficulty Engine::GetDifficulty() const { return difficulty_; }

void Engine::IncreaseDifficulty() {
  if (difficulty_ == Difficulty::kEasy) {
    difficulty_ = Difficulty::kMedium;
  } else if (difficulty_ == Difficulty::kMedium) {
    difficulty_ = Difficulty::kHard;
  } else {
    difficulty_ = Difficulty::kEasy;
  }
}

Engine::EntryState Engine::GetEntryState(int row, int col) const {
  return entry_states_[row][col];
}

void Engine::ResetEntryState(int row, int col) {
  entry_states_[row][col] = EntryState::kUnknown;
}

void Engine::CheckBoard() {
  for (size_t row = 0; row < kBoardSize; row++) {
    for (size_t col = 0; col < kBoardSize; col++) {
      if (current_entries_[row][col] == 0) {
        entry_states_[row][col] = EntryState::kUnknown;
      } else if (current_entries_[row][col] == solution_[row][col]) {
        entry_states_[row][col] = EntryState::kCorrect;
      } else {
        entry_states_[row][col] = EntryState::kWrong;
      }
    }
  }
}

bool Engine::IsGameOver() const {
  for (size_t row = 0; row < kBoardSize; row++) {
    for (size_t col = 0; col < kBoardSize; col++) {
      if (GetEntryState(row, col) != sudoku::Engine::EntryState::kCorrect) {
        return false;
      }
    }
  }
}

}  // namespace sudoku
