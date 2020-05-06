// Copyright (c) 2020 [Your Name]. All rights reserved.
#ifndef FINALPROJECT_SUDOKU_ENGINE_H_
#define FINALPROJECT_SUDOKU_ENGINE_H_

#include <array>
#include <string>
#include <vector>
#include <ratio>
#include <chrono>

using std::array;
using std::pair;

namespace sudoku {

constexpr size_t kBoardSize = 9;

class Engine {

 public:
  enum class Difficulty {
    kEasy,
    kMedium,
    kHard,
  };

  // The state of a board entry.
  // Unknown means it hasn't been explicitly checked yet for correctness
  enum class EntryState {
    kUnknown,
    kCorrect,
    kWrong,
  };

  enum class GameMode {
    kStandard,
    kTimeTrial,
    kTimeAttack
  };

  Engine();

  // Loads a random board and fill out current_entries_ with starting numbers
  void CreateGame();

  int GetEntry(pair<int, int> entry) const;
  void SetEntry(pair<int, int> entry, int num);

  // Checks if the given number has a pencil mark in the given board position
  bool IsPenciled(pair<int, int> entry, int num) const;

  // Make a pencil mark for the number if there isn't already one,
  // or undo it if there is
  void ChangePencilMark(pair<int, int> entry, int num);

  // Erase all pencil marks for a given board position
  void ClearPencilMarks(pair<int, int> entry);

  // Whether the game is in pencil mode
  bool IsPenciling() const;

  // Switch from pen to pencil mode, or vice versa
  void SwitchEntryMode();

  Difficulty GetDifficulty() const;

  // Increase the difficulty, but if already on Hard, cycle back to Easy
  void IncreaseDifficulty();
  void SetDifficulty(Difficulty difficulty);


  EntryState GetEntryState(pair<int, int> entry) const;

  // Set the state of the board position to Unknown
  void ResetEntryState(pair<int, int> entry);

  // Put the number from the solution in current_entries_
  void FillInCorrectEntry(pair<int, int> entry);

  // Update the EntryState's of the board's current entries
  void CheckBoard();

  // Return true if the current entries exactly match the solution
  bool IsGameOver() const;

  int GetGameTime() const;
  void SetStartTime(std::chrono::time_point<std::chrono::system_clock> time);

  // Sets game time to the difference between the current and start times
  void UpdateGameTime();

  GameMode GetGameMode() const;
  void SetGameMode(GameMode mode);

  int GetGamesCompleted() const;
  void IncreaseGamesCompleted();

  // Clear instance variables
  void ResetGame();

 private:
  // Gets data from a .json file about the starting board and solution
  void ImportGameBoard();

  // File path to the game's .json file
  std::string board_path_;

  // General info about the game
  Difficulty difficulty_;
  GameMode game_mode_;
  bool is_penciling_;
  int game_time_;
  int games_completed_;
  std::chrono::time_point<std::chrono::system_clock> start_time_;

  // Info about each board position
  array<array<int, kBoardSize>, kBoardSize> current_entries_;
  array<array<EntryState, kBoardSize>, kBoardSize> entry_states_;
  array<array<int, kBoardSize>, kBoardSize> solution_;
  array<array<array<bool, kBoardSize>,kBoardSize>, kBoardSize> pencil_marks_;

  // File paths of possible games
  std::vector<std::string> easy_boards_;
  std::vector<std::string> medium_boards_;
  std::vector<std::string> hard_boards_;
};
}  // namespace sudoku


#endif // FINALPROJECT_SUDOKU_ENGINE_H_
