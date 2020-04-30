// Copyright (c) 2020 CS126SP20. All rights reserved.

#ifndef FINALPROJECT_APPS_MYAPP_H_
#define FINALPROJECT_APPS_MYAPP_H_

#include <cinder/app/App.h>
#include <cinder/gl/Texture.h>

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
  void DrawGrid() const;
  void DrawGameScreen() const;
  void PrintBoardEntries() const;
  void DrawMenu() const;
  void PrintGameModes() const;
  void DrawSettings() const;
  GameState state_;
  ci::vec2 mouse_pos_;
  ci::vec2 window_center_;
  pair<int, int> selected_box_;
  int default_text_size_;
  sudoku::Engine engine_;
  bool want_instructions_;
  vector<pair<ci::vec2, ci::vec2>> game_start_buttons_;
  pair<ci::vec2, ci::vec2> difficulty_button_;
  pair<ci::vec2, ci::vec2> instructions_button_;
  pair<ci::vec2, ci::vec2> menu_return_button_;
  pair<ci::vec2, ci::vec2> entry_mode_indicator_;
  array<array<pair<ci::vec2, ci::vec2>, kBoardSize>, kBoardSize> game_grid_;
  vector<string> game_modes_;
  array<ci::gl::Texture2dRef, 2> entry_type_images_;
};

}  // namespace myapp

#endif  // FINALPROJECT_APPS_MYAPP_H_
