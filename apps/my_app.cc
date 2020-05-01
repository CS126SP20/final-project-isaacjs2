// Copyright (c) 2020 [Your Name]. All rights reserved.

#include "my_app.h"

#include <cinder/Font.h>
#include <cinder/ImageIo.h>
#include <cinder/Path2d.h>
#include <cinder/Text.h>
#include <cinder/Vector.h>
#include <cinder/app/App.h>
#include <cinder/gl/Texture.h>
#include <cinder/gl/draw.h>
#include <cinder/gl/gl.h>
#include <fstream>
#include <sudoku/engine.h>

namespace myapp {

using cinder::app::KeyEvent;
using Difficulty = sudoku::Engine::Difficulty;
using EntryState = sudoku::Engine::EntryState;
using sudoku::kBoardSize;

MyApp::MyApp()
    : state_{GameState::kMenu},
    mouse_pos_{ci::vec2(-1, -1)},
      win_center_{getWindowCenter()},
      sel_box_{-1, -1},
      def_text_size_{30},
      want_instructions_{true},
    game_modes_{{"Standard", "Time Attack", "Time Trial"}}
    {}

void MyApp::setup() {
  ci::gl::enableDepthWrite();
  ci::gl::enableDepthRead();

  SetupMenu();
  SetupGameScreen();
}

void MyApp::SetupMenu() {
  // Record the positions of the menu buttons
  for (size_t i = 0; i < game_modes_.size(); i++) {
    ci::vec2 top_left(win_center_.x - 120,
                      win_center_.y - 120 + i * 90);
    ci::vec2 bottom_right(win_center_.x + 120,
                          win_center_.y - 60 + i * 90);

    std::pair<ci::vec2, ci::vec2> button_bounds(top_left, bottom_right);
    game_start_btns_.push_back(button_bounds);
  }

  // Record the position of other menu buttons
  ci::vec2 diff_button_tl(getWindowBounds().x1 + 10,
                          getWindowBounds().y2 - 60);
  ci::vec2 diff_button_br(getWindowBounds().x1 + 130,
                          getWindowBounds().y2 - 10);
  difficulty_btn_ = {diff_button_tl, diff_button_br};

  ci::vec2 instr_button_tl(getWindowBounds().x2 - 110,
                           getWindowBounds().y2 - 60);
  ci::vec2 instr_button_br(getWindowBounds().x2 - 10,
                           getWindowBounds().y2 - 10);
  instructions_btn = {instr_button_tl, instr_button_br};

}

void MyApp::SetupGameScreen() {
  // Load entry mode images
  ci::gl::Texture2dRef marker_image = ci::gl::Texture2d::create(
      ci::loadImage(loadAsset("marker.png")));
  entry_type_images_[0] = marker_image;

  ci::gl::Texture2dRef pencil_image = ci::gl::Texture2d::create(
      ci::loadImage(loadAsset("pencil2.png")));
  entry_type_images_[1] = pencil_image;

  SetupGameBoard();

  // Record the positions of other boxes on game screen
  menu_return_btn_.first = {5, 5};
  menu_return_btn_.second = {105, 55};

  check_board_btn.first = {game_grid_[0][0].first.x,
                            game_grid_[kBoardSize -1][0].second.y + 5};
  check_board_btn.second = {game_grid_[0][0].first.x + 100,
                            game_grid_[kBoardSize -1][0].second.y + 55};

  entry_mode_indicator_.first = {win_center_.x - 50,
                                 getWindowSize().y - 100};
  entry_mode_indicator_.second = {win_center_.x + 50,
                                      getWindowSize().y};
}

void MyApp::SetupGameBoard() {
  float tile_size = std::floor(600 / kBoardSize);
  float left_bound = win_center_.x - ((float) kBoardSize / 2) * tile_size;
  float top_bound = win_center_.y - ((float) kBoardSize / 2) * tile_size;

  for (size_t row = 0; row < kBoardSize; row++) {
    for (size_t col = 0; col < kBoardSize; col++) {
      ci::vec2 top_left(left_bound + col * tile_size,
                        top_bound + row * tile_size);
      ci::vec2 bottom_right(left_bound + (col + 1) * tile_size,
                            top_bound + (row + 1) * tile_size);

      game_grid_[row][col] = {top_left, bottom_right};
    }

  }
}

void MyApp::update() {
  mouse_pos_ = getMousePos() - getWindowPos();
}

void DrawBox(std::pair<ci::vec2, ci::vec2> bounds, const ci::Color& color) {
  ci::gl::color(color);

  ci::Path2d box;
  box.moveTo(bounds.first);
  box.lineTo(ci::vec2(bounds.second.x, bounds.first.y));
  box.lineTo(bounds.second);
  box.lineTo(ci::vec2(bounds.first.x, bounds.second.y));

  box.close();
  ci::gl::draw(box);
}

template <typename C>
void PrintText(const std::string& text,
               const C& color,
               const cinder::ivec2& size,
               const cinder::vec2& loc,
               int font_size) {
  cinder::gl::enableAlphaBlending();
  cinder::gl::color(color);
  const char font[] = "Arial";

  auto box = ci::TextBox()
      .alignment(ci::TextBox::CENTER)
      .font(cinder::Font(font, font_size))
      .size(size)
      .color(color)
      .backgroundColor(ci::ColorA(0, 0, 0, 0))
      .text(text);

  const auto box_size = box.getSize();
  const cinder::vec2 locp = {loc.x - box_size.x / 2,
                             loc.y - box_size.y / 2};
  const auto surface = box.render();
  const auto texture = cinder::gl::Texture::create(surface);
  cinder::gl::draw(texture, locp);
}

void MyApp::draw() {
  cinder::gl::enableAlphaBlending();
  cinder::gl::clear(ci::Color((float) 188/256,
                                  (float) 188/256,
                                   (float) 188/256));

  if (state_ == GameState::kMenu) {
    DrawMenu();
  } else if (state_ == GameState::kPlaying) {
    // Highlight the box that the player has selected
    if (sel_box_.first != -1) {
      ci::Color color(1, 0, 0);
      DrawBox(game_grid_[sel_box_.first][sel_box_.second], color);
      DrawBox({
        ci::vec2(
            game_grid_[sel_box_.first][sel_box_.second].first.x - 1,
            game_grid_[sel_box_.first][sel_box_.second].first.y - 1),
        ci::vec2(
            game_grid_[sel_box_.first][sel_box_.second].second.x + 1,
            game_grid_[sel_box_.first][sel_box_.second].second.y + 1)},
              color);
      DrawBox({
        ci::vec2(
            game_grid_[sel_box_.first][sel_box_.second].first.x + 1,
            game_grid_[sel_box_.first][sel_box_.second].first.y + 1),
        ci::vec2(
            game_grid_[sel_box_.first][sel_box_.second].second.x - 1,
            game_grid_[sel_box_.first][sel_box_.second].second.y - 1)},
              color);
    }

    DrawGameScreen();
  }
}

void DrawLine(float x1, float y1, float x2, float y2, const ci::Color& color) {
  ci::gl::color(color);

  ci::Path2d line;
  line.moveTo(x1, y1);
  line.lineTo(x2, y2);
  line.close();
  ci::gl::draw(line);
}

void MyApp::DrawMenu() const {
  PrintText("Sudoku!",
            ci::Color::black(),
            ci::vec2(500, 100),
            ci::vec2(win_center_.x, win_center_.y - 250),
            100);

  PrintGameModes();

  // Draw game start buttons
  for (const auto& button : game_start_btns_) {
    DrawBox(button, ci::Color(0, 0, 1));
  }

  DrawSettings();
}

void MyApp::PrintGameModes() const {
  for (size_t i = 0; i < game_modes_.size(); i++) {
    PrintText(game_modes_[i],
              ci::Color(1, 0, 0),
              ci::vec2(200, 25),
              ci::vec2(win_center_.x,
                        win_center_.y - 90 + (float) 90 * i),
              def_text_size_);
  }
}

void MyApp::DrawSettings() const {
  // Draw difficulty picker
  ci::Color diff_color;
  std::string difficulty;
  if (engine_.GetDifficulty() == Difficulty::kEasy) {
    diff_color = ci::Color(0, 1, 0);
    difficulty = "Easy";
  } else if (engine_.GetDifficulty() == Difficulty::kMedium) {
    diff_color = ci::Color(1, 1, 0);
    difficulty = "Medium";
  } else if (engine_.GetDifficulty() == Difficulty::kHard) {
    diff_color = ci::Color(1, 0, 0);
    difficulty = "Hard";
  }
  PrintText("Difficulty",
            ci::Color::black(),
            ci::vec2(200, 35),
            ci::vec2(difficulty_btn_.first.x + 55,
                         difficulty_btn_.first.y - 20),
            40);
  PrintText(difficulty,
            diff_color,
            ci::vec2(120, 40),
            ci::vec2(difficulty_btn_.first.x + 60,
                         difficulty_btn_.first.y + 25),
            40);
  DrawBox(difficulty_btn_, ci::Color::black());

  // Draw instructions toggle
  ci::Color instr_color;
  string text;
  if (want_instructions_) {
    instr_color = ci::Color(0, 1, 0);
    text = "ON";
  } else {
    instr_color = ci::Color(1, 0, 0);
    text = "OFF";
  }
  PrintText(text,
            instr_color,
            ci::vec2(80, 40),
            ci::vec2(instructions_btn.first.x + 50,
                         instructions_btn.first.y + 25),
            40);

  PrintText("Instructions",
            ci::Color::black(),
            ci::vec2(200, 35),
            ci::vec2(instructions_btn.second.x - 75,
                         instructions_btn.first.y - 20),
            40);
  DrawBox(instructions_btn, ci::Color::black());
}

void MyApp::DrawGrid() const {
  float tile_size = std::floor(600 / kBoardSize);
  ci::Color color = ci::Color::black();

  for (const auto& row : game_grid_) {
    for (const auto& col : row) {
      DrawBox(col, color);
    }
  }

  // Make the lines around each 3x3 box thicker
  for (size_t i = 0; i < kBoardSize + 1; i+= 3) {
    // Vertical lines
    DrawLine(game_grid_[0][0].first.x + i * tile_size - 1,
                 game_grid_[0][0].first.y,
             game_grid_[0][0].first.x + i * tile_size - 1,
             game_grid_[kBoardSize - 1][0].second.y,
             color);
    DrawLine(game_grid_[0][0].first.x + i * tile_size + 1,
                 game_grid_[0][0].first.y,
             game_grid_[0][0].first.x + i * tile_size + 1,
             game_grid_[kBoardSize - 1][0].second.y,
             color);

    // Horizontal lines
    DrawLine(game_grid_[0][0].first.x,
         game_grid_[0][0].first.y + i * tile_size - 1,
         game_grid_[0][kBoardSize - 1].second.x,
         game_grid_[0][0].first.y + i * tile_size - 1,
         color);
    DrawLine(game_grid_[0][0].first.x,
         game_grid_[0][0].first.y + i * tile_size + 1,
         game_grid_[0][kBoardSize - 1].second.x,
         game_grid_[0][0].first.y + i * tile_size + 1,
         color);
  }
}

void MyApp::DrawGameScreen() const {
  // Draw back to menu button
  DrawBox(menu_return_btn_, ci::Color(0, 0, 1));
  PrintText("Menu",
      ci::Color(1, 0, 0),
       ci::vec2(95, 50),
       ci::vec2(55, 30),
            def_text_size_);

  // Draw check board button
  DrawBox(check_board_btn, ci::Color::black());
  PrintText("Check Board",
            ci::Color::black(),
            ci::vec2(95, 45),
            ci::vec2(check_board_btn.first.x + 50,
                         check_board_btn.first.y + 25),
            def_text_size_);

  // Draw entry mode indicator
  ci::Area box(entry_mode_indicator_.first,
               entry_mode_indicator_.second);
  if (engine_.IsPenciling()) {
    ci::gl::draw(entry_type_images_[1], box);
  } else {
    ci::gl::draw(entry_type_images_[0], box);
  }

  DrawGrid();

  PrintBoardEntries();

  if (want_instructions_) {
    DrawGameInstructions();
  }
}

void MyApp::PrintBoardEntries() const {
  // Print pencil marks and board entries
  float tile_size = std::floor(600 / kBoardSize);

  for (size_t row = 0; row < kBoardSize; row++) {
    for (size_t col = 0; col < kBoardSize; col++) {
      if (engine_.GetEntry(row, col) == 0) {
        // Print pencil marks
        for (size_t num = 1; num < kBoardSize + 1; num++) {
          if (engine_.IsPenciled(row, col, num)) {
            ci::vec2 mark_loc(game_grid_[row][col].first.x
                                  + tile_size / 6
                                  + ((num - 1) % 3) * tile_size / 3,
                              game_grid_[row][col].first.y
                                  + tile_size / 6
                                  + ((num - 1) / 3) * tile_size / 3);

            PrintText(std::to_string(num),
                      ci::Color::black(),
                      ci::vec2(tile_size / 3, tile_size / 3),
                      mark_loc,
                      tile_size / 3);
          }
        }
      } else {
        // Print board entries
        ci::vec2 text_size(35, 35);
        ci::vec2 text_loc(game_grid_[row][col].first.x + tile_size / 2,
                          game_grid_[row][col].first.y + tile_size / 2);

        ci::Color color(ci::Color::black());
        switch (engine_.GetEntryState(row, col)) {
          case sudoku::Engine::EntryState::kCorrect :
            color = ci::Color(0, 0, 1);
            break;
          case sudoku::Engine::EntryState::kWrong :
            color = ci::Color(1, 0, 0);
            break;
          case sudoku::Engine::EntryState::kUnknown :
            color = ci::Color::black();
            break;
        }

        PrintText(std::to_string(engine_.GetEntry(row, col)),
                  color,
                  text_size,
                  text_loc,
                  50);
      }
    }
  }
}

void MyApp::DrawGameInstructions() const {
  PrintText("Welcome to Sudoku! To solve the puzzle,",
            ci::Color::black(),
            ci::vec2(600, 30),
            ci::vec2(win_center_.x, getWindowBounds().y1 + 20),
            def_text_size_);
  PrintText("fill every row, column, and box with the numbers 1 to 9.",
            ci::Color::black(),
            ci::vec2(600, 30),
            ci::vec2(win_center_.x, getWindowBounds().y1 + 50),
            def_text_size_);
  PrintText("Use your mouse and number pad to fill in the board.",
            ci::Color::black(),
            ci::vec2(600, 30),
            ci::vec2(win_center_.x, getWindowBounds().y1 + 80),
            def_text_size_);

  PrintText("This symbol shows what entry mode you're in.",
            ci::Color::black(),
            ci::vec2(350, 20),
            ci::vec2(620, getWindowBounds().y2 - 90),
            20);
  PrintText("Pen mode is for filling in the board, and",
            ci::Color::black(),
            ci::vec2(350, 20),
            ci::vec2(620, getWindowBounds().y2 - 70),
            20);
  PrintText("pencil mode is for making notes when you're",
            ci::Color::black(),
            ci::vec2(350, 20),
            ci::vec2(620, getWindowBounds().y2 - 50),
            20);
  PrintText("not sure what number goes in a box yet.",
            ci::Color::black(),
            ci::vec2(350, 20),
            ci::vec2(620, getWindowBounds().y2 - 30),
            20);
}

void MyApp::keyDown(KeyEvent event) {
  // Erase the current contents of a box
  if (event.getCode() == KeyEvent::KEY_BACKSPACE
      && sel_box_.first != -1
      && engine_.GetEntryState(sel_box_.first, sel_box_.second) != sudoku::Engine::EntryState::kCorrect) {
    if (engine_.GetEntry(sel_box_.first, sel_box_.second) == 0) {
      engine_.ClearPencilMarks(sel_box_.first, sel_box_.second);
    } else {
      engine_.SetEntry(sel_box_.first, sel_box_.second, 0);
    }
  }

  if (sel_box_.first != -1) {
    // Update current pencil marks
    if (engine_.IsPenciling()
        && engine_.GetEntry(sel_box_.first, sel_box_.second) == 0) {
      for (size_t i = 1; i < kBoardSize + 1; i++) {
        if (event.getCode() == i + 48) {
          engine_.Pencil(sel_box_.first, sel_box_.second, i);

          break;
        }
      }
    // Update board entries
    } else if (!engine_.IsPenciling()
               && engine_.GetEntryState(sel_box_.first, sel_box_.second) != sudoku::Engine::EntryState::kCorrect) {
      for (size_t i = 1; i < kBoardSize + 1; i++) {
        if (event.getCode() == i + 48) {
          engine_.SetEntry(sel_box_.first, sel_box_.second, i);
          engine_.ResetEntryState(sel_box_.first, sel_box_.second);

          break;
        }
      }
    }
  }
}

bool IsMouseInBox(const ci::vec2& mouse_pos, std::pair<ci::vec2, ci::vec2> box_bounds) {
  return mouse_pos.x > box_bounds.first.x
         && mouse_pos.x < box_bounds.second.x
         && mouse_pos.y > box_bounds.first.y
         && mouse_pos.y < box_bounds.second.y;
}

void MyApp::mouseDown(ci::app::MouseEvent event) {
  if (event.isLeft()) {
    if (state_ == GameState::kMenu) { //combine into for loop
      if (IsMouseInBox(mouse_pos_, game_start_btns_[0])) {
        state_ = GameState::kPlaying;
        engine_.CreateGame(Difficulty::kEasy);
      } else if (IsMouseInBox(mouse_pos_, game_start_btns_[1])) {
        state_ = GameState::kPlaying;
        engine_.CreateGame(Difficulty::kEasy);
      } else if (IsMouseInBox(mouse_pos_, game_start_btns_[2])) {
        state_ = GameState::kPlaying;
        engine_.CreateGame(Difficulty::kEasy);
      }

      // Change difficulty
      if (IsMouseInBox(mouse_pos_, difficulty_btn_)) {
        engine_.IncreaseDifficulty();
      }

      // Toggle instructions
      if (IsMouseInBox(mouse_pos_, instructions_btn)) {
        want_instructions_ = !want_instructions_;
      }
    } else if (state_ == GameState::kPlaying) {
      if (IsMouseInBox(mouse_pos_, menu_return_btn_)) {
        state_ = GameState::kMenu;

        sel_box_ = {-1, -1};
      }

      if (IsMouseInBox(mouse_pos_, check_board_btn)) {
        engine_.CheckBoard();
      }

      for (size_t row = 0; row < kBoardSize; row++) {
        for (size_t col = 0; col < kBoardSize; col++) {
          if (IsMouseInBox(mouse_pos_, game_grid_[row][col])) {
            sel_box_ = {row, col};
          }
        }
      }
    }
  }

  if (event.isRight() && state_ == GameState::kPlaying) {
    engine_.SwitchEntryMode();
  }
}
}  // namespace myapp
