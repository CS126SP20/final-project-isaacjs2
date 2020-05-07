// Copyright (c) 2020 CS126SP20. All rights reserved.

#include <sudoku/leaderboard.h>
#include <sudoku/player.h>

#include <sqlite_modern_cpp.h>

#include <string>
#include <vector>

namespace sudoku {

using std::string;
using std::vector;

// See examples: https://github.com/SqliteModernCpp/sqlite_modern_cpp/tree/dev

LeaderBoard::LeaderBoard(const string& db_path) : db_{db_path} {
  try {
    db_ << "CREATE TABLE if not exists leaderboard (\n"
           "  name  TEXT NOT NULL,\n"
           "  time INTEGER NOT NULL\n"
           "  mode TEXT NOT NULL\n"
           "  difficulty TEXT NOT NULL\n"
           ");";
  } catch (const sqlite::sqlite_exception& e) {
    std::cerr  << e.get_code() << ": " << e.what() << " during "
               << e.get_sql() << std::endl;
  }
}

void LeaderBoard::AddTimeToLeaderBoard(const Player& player,
                                       std::string mode,
                                       std::string difficulty) {
  try {
    db_ << "insert into leaderboard (name, time, mode, difficulty) "
           "values (?,?,?,?);"
        << player.name
        << player.time
        << mode
        << difficulty;
  } catch (const sqlite::sqlite_exception& e) {
    std::cerr  << e.get_code() << ": " << e.what() << " during "
               << e.get_sql() << std::endl;
  }
}

vector<Player> GetPlayers(sqlite::database_binder* rows) {
  vector<Player> players;

  for (auto&& row : *rows) {
    string name;
    size_t time;
    row >> name >> time;
    Player player = {name, time};
    players.push_back(player);
  }

  return players;
}

vector<Player> LeaderBoard::RetrieveBestTimes(const size_t limit,
                                              std::string mode,
                                              std::string difficulty) {
  try {
    auto rows = db_ << "select name,time from leaderboard "
                       "where mode = ? and difficulty = ? "
                       "order by time asc "
                       "limit ?;"
                    << mode
                    << difficulty
                    << limit;
    return GetPlayers(&rows);
  } catch (const sqlite::sqlite_exception& e) {
    std::cerr << e.get_code() << ": " << e.what() << " during " << e.get_sql()
              << std::endl;
  }

  vector<Player> rows;
  return rows;
}
}  // namespace sudoku
