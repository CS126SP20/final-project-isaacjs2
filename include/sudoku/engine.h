// Copyright (c) 2020 [Your Name]. All rights reserved.
#ifndef FINALPROJECT_SUDOKU_ENGINE_H_
#define FINALPROJECT_SUDOKU_ENGINE_H_

#include <string>
namespace sudoku {

class Engine {
 public:
  Engine(std::string path);

 private:
  void ImportGameBoard();

  std::string board_path_;

};
}  // namespace sudoku


#endif // FINALPROJECT_SUDOKU_ENGINE_H_
