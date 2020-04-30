// Copyright (c) 2020 [Your Name]. All rights reserved.
#ifndef FINALPROJECT_SUDOKU_ENGINE_H_
#define FINALPROJECT_SUDOKU_ENGINE_H_

#include <array>
#include <string>

using std::array;

namespace sudoku {

constexpr size_t kBoardSize = 9;

class Engine {
 public:
  Engine();
  void CreateGame(std::string path);
  int GetEntry(int row, int col) const;
  void SetEntry(int row, int col, int num);
  bool IsPenciled(int row, int col, int num) const;
  void Pencil(int row, int col, int num);
  void ClearPencilMarks(int row, int col);

 private:
  void ImportGameBoard();

  std::string board_path_;
  array<array<int, kBoardSize>, kBoardSize> current_entries_;
  array<array<int, kBoardSize>, kBoardSize> solution_;
  array<array<array<bool, kBoardSize>,kBoardSize>, kBoardSize> pencil_marks_;

  std::vector<std::string> easy_boards_;
  std::vector<std::string> medium_boards_;
  std::vector<std::string> hard_boards_;
};
}  // namespace sudoku


#endif // FINALPROJECT_SUDOKU_ENGINE_H_
