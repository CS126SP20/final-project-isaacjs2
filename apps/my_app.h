// Copyright (c) 2020 CS126SP20. All rights reserved.

#ifndef FINALPROJECT_APPS_MYAPP_H_
#define FINALPROJECT_APPS_MYAPP_H_

#include <cinder/app/App.h>
#include <cinder/app/KeyEvent.h>
#include <cinder/gl/Texture.h>
#include <sudoku/leaderboard.h>

#include <array>
#include <string>
#include <vector>

#include "../include/sudoku/engine.h"

using sudoku::kBoardSize;
using std::array;
using std::string;
using std::vector;
using std::pair;

namespace myapp {

enum class GameState {
  kMenu,
  kPlaying,
  kGameOver,
};

class MyApp : public cinder::app::App {
 public:
  MyApp();
  void setup() override;
  void update() override;
  void draw() override;
  void keyDown(cinder::app::KeyEvent) override;
  void mouseDown(cinder::app::MouseEvent) override;

 private:
  void SetupMenu();
  void SetupGameScreen();
  void SetupGameBoard();
  void SetupGameOver();
  void UpdateLeaderboard();
  void DrawGrid() const;
  void DrawGameScreen();
  void PrintGameInstructions() const;
  void PrintBoardEntries() const;
  void HighlightSelectedBox() const;
  void DrawMenu() const;
  void PrintGameModes() const;
  void DrawSettings() const;
  void PrintMenuInstructions() const;
  void DrawGameOver() const;
  void DrawLeaderboard() const;
  void ExecuteArrowKeyMovement(int key_code);
  void UpdatePlayerName(cinder::app::KeyEvent event);
  void ExecuteGameClick();
  void StartNewGame(int mode);
  void ResetApp();
  string GetModeAsString() const;
  string GetDifficultyAsString() const;
  GameState state_;
  ci::vec2 mouse_pos_;
  ci::vec2 win_center_;
  pair<int, int> sel_box_;
  sudoku::Engine engine_;
  sudoku::LeaderBoard leaderboard_;
  vector<sudoku::Player> top_players_;
  bool want_instructions_;
  bool is_entering_name_;
  string player_name_;
  vector<pair<ci::vec2, ci::vec2>> game_start_btns_;
  pair<ci::vec2, ci::vec2> difficulty_btn_;
  pair<ci::vec2, ci::vec2> instructions_btn;
  pair<ci::vec2, ci::vec2> menu_return_btn_;
  pair<ci::vec2, ci::vec2> hint_btn_;
  pair<ci::vec2, ci::vec2> check_board_btn;
  pair<ci::vec2, ci::vec2> entry_mode_indicator_;
  array<array<pair<ci::vec2, ci::vec2>, kBoardSize>, kBoardSize> game_grid_;
  vector<string> game_modes_;
  array<ci::gl::Texture2dRef, 2> entry_type_images_;
  pair<ci::vec2, ci::vec2> play_again_btn;
};
}  // namespace myapp

#endif  // FINALPROJECT_APPS_MYAPP_H_
