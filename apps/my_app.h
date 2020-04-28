// Copyright (c) 2020 CS126SP20. All rights reserved.

#ifndef FINALPROJECT_APPS_MYAPP_H_
#define FINALPROJECT_APPS_MYAPP_H_

#include <cinder/app/App.h>
#include <cinder/gl/Texture.h>

#include <array>

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
  void DrawGrid() const;
  void DrawGameScreen() const;
  void PrintBoardEntries() const;
  void DrawMenu() const;
  void PrintGameModes() const;
  void ResetGameBoard();
  GameState state_;
  ci::vec2 mouse_pos_;
  ci::vec2 window_center_;
  float board_size_;
  int selected_box_;
  int default_text_size_;
  bool is_penciling_;
  std::vector<std::vector<ci::vec2>> menu_buttons_;
  std::vector<std::vector<ci::vec2>> game_buttons_;
  std::vector<std::vector<ci::vec2>> game_grid_;
  std::vector<std::string> game_modes_;
  std::vector<std::string> board_entries_;
  std::vector<std::vector<std::string>> board_pencil_marks_;
  std::vector<ci::gl::Texture2dRef> entry_type_images_;
};

}  // namespace myapp

#endif  // FINALPROJECT_APPS_MYAPP_H_
