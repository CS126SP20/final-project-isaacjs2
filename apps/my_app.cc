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
using sudoku::kBoardSize;

MyApp::MyApp()
    : state_{GameState::kMenu},
    mouse_pos_{ci::vec2(-1, -1)},
    window_center_{getWindowCenter()},
    selected_box_{-1, -1},
    default_text_size_{30},
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
    ci::vec2 top_left(window_center_.x - 120,
                      window_center_.y - 120 + i * 90);
    ci::vec2 bottom_right(window_center_.x + 120,
                          window_center_.y - 60 + i * 90);

    std::pair<ci::vec2, ci::vec2> button_bounds(top_left, bottom_right);
    game_start_buttons_.push_back(button_bounds);
  }

  // Record the position of other menu buttons
  ci::vec2 diff_button_tl(getWindowBounds().x1 + 10, getWindowBounds().y2 - 60);
  ci::vec2 diff_button_br(getWindowBounds().x1 + 130, getWindowBounds().y2 - 10);
  difficulty_button_ = {diff_button_tl, diff_button_br};

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
  menu_return_button_.first = {5, 5};
  menu_return_button_.second = {105, 55};

  entry_mode_indicator_.first = {window_center_.x - 50,
                                 getWindowSize().y - 100};
  entry_mode_indicator_.second = {window_center_.x + 50, getWindowSize().y};
}

void MyApp::SetupGameBoard() {
  float tile_size = std::floor(600 / kBoardSize);
  float left_bound = window_center_.x - ((float) kBoardSize / 2) * tile_size;
  float top_bound = window_center_.y - ((float) kBoardSize / 2) * tile_size;

  for (size_t row = 0; row < kBoardSize; row++) {
    for (size_t col = 0; col < kBoardSize; col++) {
      ci::vec2 top_left(left_bound + col * tile_size,
                        top_bound + row * tile_size);
      ci::vec2 bottom_right(left_bound + (col + 1) * tile_size,
                            top_bound + (row + 1) * tile_size);

      std::pair<ci::vec2, ci::vec2> box_bounds(top_left, bottom_right);
      game_grid_[row][col] = box_bounds;
    }

  }
}

void MyApp::update() {
  mouse_pos_ = getMousePos() - getWindowPos();
}

void DrawBox(const ci::vec2& top_left,
             const ci::vec2& bottom_right,
             const ci::Color& color) {
  ci::gl::color(color);

  ci::Path2d box;
  box.moveTo(top_left);
  box.lineTo(ci::vec2(bottom_right.x, top_left.y));
  box.lineTo(bottom_right);
  box.lineTo(ci::vec2(top_left.x, bottom_right.y));

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
    if (selected_box_.first != -1) {
      ci::Color color(1, 0, 0);
      DrawBox(game_grid_[selected_box_.first][selected_box_.second].first,
              game_grid_[selected_box_.first][selected_box_.second].second,
              color);
      DrawBox(ci::vec2(game_grid_[selected_box_.first][selected_box_.second].first.x - 1,
                               game_grid_[selected_box_.first][selected_box_.second].first.y - 1),
          ci::vec2(game_grid_[selected_box_.first][selected_box_.second].second.x + 1,
                               game_grid_[selected_box_.first][selected_box_.second].second.y + 1),
                      color);
      DrawBox(ci::vec2(game_grid_[selected_box_.first][selected_box_.second].first.x + 1,
                               game_grid_[selected_box_.first][selected_box_.second].first.y + 1),
          ci::vec2(game_grid_[selected_box_.first][selected_box_.second].second.x - 1,
                               game_grid_[selected_box_.first][selected_box_.second].second.y - 1),
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
  PrintGameModes();

  // Draw game start buttons
  for (size_t i = 0; i < game_modes_.size(); i++) {
    DrawBox(ci::vec2(window_center_.x - 120,
                             window_center_.y - 120 + i * 90),
        ci::vec2(window_center_.x + 120,
                             window_center_.y - 60 + i * 90),
        ci::Color(0, 0, 1));
  }

  // Draw difficulty picker
  ci::Color color;
  std::string difficulty;
  if (engine_.GetDifficulty() == Difficulty::kEasy) {
    color = ci::Color(0, 1, 0);
    difficulty = "Easy";
  } else if (engine_.GetDifficulty() == Difficulty::kMedium) {
    color = ci::Color(1, 1, 0);
    difficulty = "Medium";
  } else if (engine_.GetDifficulty() == Difficulty::kHard) {
    color = ci::Color(1, 0, 0);
    difficulty = "Hard";
  }
  PrintText("Difficulty",
            ci::Color::black(),
            ci::vec2(200, 35),
            ci::vec2(difficulty_button_.first.x + 55,
                         difficulty_button_.first.y - 20),
            40);
  PrintText(difficulty,
            color,
            ci::vec2(120, 40),
            ci::vec2(difficulty_button_.first.x + 60,
                         difficulty_button_.first.y + 25),
            40);
  DrawBox(difficulty_button_.first, difficulty_button_.second, ci::Color::black());
}

void MyApp::PrintGameModes() const {
  for (size_t i = 0; i < game_modes_.size(); i++) {
    PrintText(game_modes_[i],
              ci::Color(1, 0, 0),
              ci::vec2(200, 25),
              ci::vec2(window_center_.x,
                        window_center_.y - 90 + (float) 90 * i),
              default_text_size_);
  }
}

void MyApp::DrawGrid() const {
  float tile_size = std::floor(600 / kBoardSize);
  ci::Color color = ci::Color::black();

  for (auto row : game_grid_) {
    for (auto col : row) {
      DrawBox(col.first, col.second, color);
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
  DrawBox(menu_return_button_.first,
          menu_return_button_.second,
          ci::Color(0, 0, 1));
  PrintText("Menu",
      ci::Color(1, 0, 0),
       ci::vec2(95, 50),
       ci::vec2(55, 30),
       default_text_size_);

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
        if (engine_.IsStartingNumber(row, col)) {
          color = ci::Color(0, 0, 1);
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

void MyApp::keyDown(KeyEvent event) {
  // Erase the current contents of a box
  if (event.getCode() == KeyEvent::KEY_BACKSPACE
      && selected_box_.first != -1
      && !engine_.IsStartingNumber(selected_box_.first, selected_box_.second)) {
    if (engine_.GetEntry(selected_box_.first, selected_box_.second) == 0) {
      engine_.ClearPencilMarks(selected_box_.first, selected_box_.second);
    } else {
      engine_.SetEntry(selected_box_.first, selected_box_.second, 0);
    }
  }

  if (selected_box_.first != -1) {
    // Update current pencil marks
    if (engine_.IsPenciling()
        && engine_.GetEntry(selected_box_.first, selected_box_.second) == 0) {
      for (size_t i = 1; i < kBoardSize + 1; i++) {
        if (event.getCode() == i + 48) {
          engine_.Pencil(selected_box_.first, selected_box_.second, i);

          break;
        }
      }
    // Update board entries
    } else if (!engine_.IsPenciling()
               && !engine_.IsStartingNumber(selected_box_.first,
                                            selected_box_.second)) {
      for (size_t i = 1; i < kBoardSize + 1; i++) {
        if (event.getCode() == i + 48) {
          engine_.SetEntry(selected_box_.first, selected_box_.second, i);

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
      if (IsMouseInBox(mouse_pos_, game_start_buttons_[0])) {
        state_ = GameState::kPlaying;
        engine_.CreateGame(Difficulty::kEasy);
        // engine.start("Standard);
      } else if (IsMouseInBox(mouse_pos_, game_start_buttons_[1])) {
        state_ = GameState::kPlaying;
        engine_.CreateGame(Difficulty::kEasy);
        // engine.start("Time Attack);
      } else if (IsMouseInBox(mouse_pos_, game_start_buttons_[2])) {
        state_ = GameState::kPlaying;
        engine_.CreateGame(Difficulty::kEasy);
        // engine.start("Time Trial);
      }

      // Change difficulty
      if (IsMouseInBox(mouse_pos_, difficulty_button_)) {
        engine_.IncreaseDifficulty();
      }
    } else if (state_ == GameState::kPlaying) {
      if (IsMouseInBox(mouse_pos_, menu_return_button_)) {
        state_ = GameState::kMenu;

        selected_box_ = {-1, -1};
      }

      for (size_t row = 0; row < kBoardSize; row++) {
        for (size_t col = 0; col < kBoardSize; col++) {
          if (IsMouseInBox(mouse_pos_, game_grid_[row][col])) {
            selected_box_ = {row, col};
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
