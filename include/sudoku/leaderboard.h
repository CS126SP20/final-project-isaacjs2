#ifndef FINALPROJECT_LEADERBOARD_H
#define FINALPROJECT_LEADERBOARD_H

#include "leaderboard.h"
#include "player.h"

#include <sqlite_modern_cpp.h>

#include <string>
#include <vector>

namespace sudoku {

class LeaderBoard {
 public:
  // Creates a new leaderboard table if it doesn't already exist.
  explicit LeaderBoard(const std::string& db_path);

  // Adds a player to the leaderboard.
  void AddTimeToLeaderBoard(const Player&);

  // Returns a list of the players with the highest scores, in decreasing order.
  // The size of the list should be no greater than `limit`.
  std::vector<Player> RetrieveBestTimes(const size_t limit);

 private:
  sqlite::database db_;
};

}  // namespace sudoku

#endif  // FINALPROJECT_LEADERBOARD_H
