// Copyright (c) 2020 [Your Name]. All rights reserved.

#include <sudoku/engine.h>

#include <chrono>
#include <fstream>

#include <nlohmann/json.hpp>

#include <ratio>
#include <utility>

#include <cinder/app/App.h>
#include <cinder/app/Platform.h>

using nlohmann::json;

using std::pair;

namespace sudoku {

Engine::Engine() : difficulty_{Difficulty::kEasy},
              game_mode_{GameMode::kStandard},
              is_penciling_{false},
              game_time_{0},
              games_completed_{0},
              easy_boards_{"easy_1.json", "easy_2.json", "easy_3.json"},
              medium_boards_{"medium_1.json", "medium_2.json", "medium_3.json"},
              hard_boards_{"hard_1.json", "hard_2.json", "hard_3.json"}
              {}

void Engine::CreateGame() {
  unsigned seed = time(nullptr);
  std::srand(seed);

  // Get a board of the right difficulty
  switch (difficulty_) {
    case Difficulty::kEasy :
      board_path_ = easy_boards_[std::rand() % 3];
      break;
    case Difficulty::kMedium :
      board_path_ = medium_boards_[std::rand() % 3];
      break;
    case Difficulty::kHard :
      board_path_ = hard_boards_[std::rand() % 3];
      break;
  }

  ImportGameBoard();

  // Mark the starting entries as correct
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

void Engine::CreateGame(std::string filepath) {
  board_path_ = filepath;

  ImportGameBoard();

  // Mark the starting entries as correct
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
  infile.open(ci::app::getAssetPath(board_path_));

  // Load the file data into a JSON object
  nlohmann::json board_data;
  infile >> board_data;

  board_data.at("board").get_to(current_entries_);
  board_data.at("solution").get_to(solution_);
}

int Engine::GetEntry(pair<int, int> entry) const {
  return current_entries_[entry.first][entry.second];
}

void Engine::SetEntry(pair<int, int> entry, int num) {
  current_entries_[entry.first][entry.second] = num;
}

bool Engine::IsPenciled(pair<int, int> entry, int num) const {
  return pencil_marks_[entry.first][entry.second][num - 1];
}

void Engine::ChangePencilMark(pair<int, int> entry, int num) {
  pencil_marks_[entry.first][entry.second][num - 1]
      = !pencil_marks_[entry.first][entry.second][num - 1];
}

void Engine::ClearPencilMarks(pair<int, int> entry) {
  for (size_t num = 0; num < kBoardSize; num++) {
    pencil_marks_[entry.first][entry.second][num] = false;
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

void Engine::SetDifficulty(Difficulty difficulty) {
  difficulty_ = difficulty;
}

Engine::EntryState Engine::GetEntryState(pair<int, int> entry) const {
  return entry_states_[entry.first][entry.second];
}

void Engine::ResetEntryState(pair<int, int> entry) {
  entry_states_[entry.first][entry.second] = EntryState::kUnknown;
}

void Engine::FillInCorrectEntry(pair<int, int> entry) {
  current_entries_[entry.first][entry.second]
                                         = solution_[entry.first][entry.second];
  entry_states_[entry.first][entry.second] = EntryState::kCorrect;
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
      if (GetEntryState({row, col}) != sudoku::Engine::EntryState::kCorrect) {
        return false;
      }
    }
  }

  return true;
}

int Engine::GetGameTime() const {
  return game_time_;
}

void Engine::SetGameMode(GameMode mode) {
  game_mode_ = mode;
}

void Engine::UpdateGameTime() {
  game_time_ = (int) std::chrono::duration_cast<std::chrono::seconds>
      (std::chrono::system_clock::now() - start_time_).count();
}

Engine::GameMode Engine::GetGameMode() const {
  return game_mode_;
}

int Engine::GetGamesCompleted() const {
  return games_completed_;
}

void Engine::IncreaseGamesCompleted() {
  games_completed_++;
}

void Engine::SetStartTime(std::chrono::time_point
                                       <std::chrono::system_clock> time) {
  start_time_ = time;
}

void Engine::ResetGame() {
  is_penciling_ = false;
  game_time_ = 0;
  game_mode_ = GameMode::kStandard;
  games_completed_ = 0;

  for (size_t row = 0; row < kBoardSize; row++) {
    for (size_t col = 0; col < kBoardSize; col++) {
      current_entries_[row][col] = 0;
      ResetEntryState({row, col});
      ClearPencilMarks({row, col});
    }
  }
}

}  // namespace sudoku
