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
  void CreateGame();
  int GetEntry(pair<int, int> entry) const;
  void SetEntry(pair<int, int> entry, int num);
  bool IsPenciled(pair<int, int> entry, int num) const;
  void ChangePencilMark(pair<int, int> entry, int num);
  void ClearPencilMarks(pair<int, int> entry);
  bool IsPenciling() const;
  void SwitchEntryMode();
  Difficulty GetDifficulty() const;
  void IncreaseDifficulty();
  void SetDifficulty(Difficulty difficulty);
  EntryState GetEntryState(pair<int, int> entry) const;
  void ResetEntryState(pair<int, int> entry);
  void FillInCorrectEntry(pair<int, int> entry);
  void CheckBoard();
  bool IsGameOver() const;
  int GetGameTime() const;
  void UpdateGameTime();
  GameMode GetGameMode() const;
  void SetGameMode(GameMode mode);
  int GetGamesCompleted() const;
  void IncreaseGamesCompleted();
  void SetStartTime(std::chrono::time_point<std::chrono::system_clock> time);
  void ResetGame();

 private:
  void ImportGameBoard();

  Difficulty difficulty_;
  GameMode game_mode_;
  std::string board_path_;
  bool is_penciling_;
  int game_time_;
  int games_completed_;
  std::chrono::time_point<std::chrono::system_clock> start_time_;
  array<array<int, kBoardSize>, kBoardSize> current_entries_;
  array<array<EntryState, kBoardSize>, kBoardSize> entry_states_;
  array<array<int, kBoardSize>, kBoardSize> solution_;
  array<array<array<bool, kBoardSize>,kBoardSize>, kBoardSize> pencil_marks_;

  std::vector<std::string> easy_boards_;
  std::vector<std::string> medium_boards_;
  std::vector<std::string> hard_boards_;
};
}  // namespace sudoku


#endif // FINALPROJECT_SUDOKU_ENGINE_H_
