#ifndef FINALPROJECT_PLAYER_H
#define FINALPROJECT_PLAYER_H

#include <string>

namespace sudoku {

struct Player {
  Player(const std::string& name, size_t time) : name(name), time(time) {}
  std::string name;
  size_t time;
};

}  // namespace sudoku

#endif  // FINALPROJECT_PLAYER_H
