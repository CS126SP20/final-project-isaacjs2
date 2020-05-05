// Copyright (c) 2020 [Your Name]. All rights reserved.
#ifndef FINALPROJECT_SUDOKU_ENGINE_H_
#define FINALPROJECT_SUDOKU_ENGINE_H_

#include <array>
#include <string>
#include <vector>
#include <ratio>
#include <chrono>

using std::array;

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

  Engine();
  void CreateGame(Difficulty difficulty);
  int GetEntry(int row, int col) const;
  void SetEntry(int row, int col, int num);
  bool IsPenciled(int row, int col, int num) const;
  void ChangePencilMark(int row, int col, int num);
  void ClearPencilMarks(int row, int col);
  bool IsPenciling() const;
  void SwitchEntryMode();
  Difficulty GetDifficulty() const;
  void IncreaseDifficulty();
  EntryState GetEntryState(int row, int col) const;
  void ResetEntryState(int row, int col);
  void CheckBoard();
  bool IsGameOver() const;
  int GetGameTime() const;
  void SetGameTime(std::chrono::duration<long long, std::ratio<1, 10000000>> time);
  void ResetGame();

 private:
  void ImportGameBoard();

  Difficulty difficulty_;
  std::string board_path_;
  bool is_penciling_;
  int game_time_;
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
