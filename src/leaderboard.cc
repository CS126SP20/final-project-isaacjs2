// Copyright (c) 2020 CS126SP20. All rights reserved.

#include <minesweeper/leaderboard.h>
#include <minesweeper/player.h>
#include <sqlite_modern_cpp.h>

#include <string>
#include <vector>

namespace minesweeper {

using std::string;
using std::vector;

// See examples: https://github.com/SqliteModernCpp/sqlite_modern_cpp/tree/dev

LeaderBoard::LeaderBoard(const string& db_path) : db_{db_path} {
  try {
    db_ << "CREATE TABLE if not exists leaderboard (\n"
           "  name  TEXT NOT NULL,\n"
           "  time INTEGER NOT NULL\n"
           ");";
  } catch (const sqlite::sqlite_exception& e) {
    std::cerr  << e.get_code() << ": " << e.what() << " during "
               << e.get_sql() << std::endl;
  }
}

void LeaderBoard::AddScoreToLeaderBoard(const Player& player) {
  try {
    db_ << "insert into leaderboard (name, score) values (?,?);"
        << player.name
        << player.time;
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

vector<Player> LeaderBoard::RetrieveHighScores(const size_t limit) {
  try {
    auto rows = db_ << "select name,time from leaderboard "
                       "order by time desc "
                       "limit ?;"
                    << limit;
    return GetPlayers(&rows);
  } catch (const sqlite::sqlite_exception& e) {
    std::cerr  << e.get_code() << ": " << e.what() << " during "
               << e.get_sql() << std::endl;
  }

  vector<Player> rows;
  return rows;
}

vector<Player> LeaderBoard::RetrieveHighScores(const Player& player,
                                               const size_t limit) {
  try {
    auto rows = db_ << "select name,score from leaderboard "
                       "where name = ? "
                       "order by time desc "
                       "limit ?;"
                    << player.name
                    << limit;
    return GetPlayers(&rows);
  } catch (const sqlite::sqlite_exception& e) {
    std::cerr  << e.get_code() << ": " << e.what() << " during "
               << e.get_sql() << std::endl;
  }

  vector<Player> rows;
  return rows;
}

}  // namespace minesweeper
