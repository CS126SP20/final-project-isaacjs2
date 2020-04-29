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

namespace myapp {

using cinder::app::KeyEvent;

MyApp::MyApp()
    : state_{GameState::kMenu},
    mouse_pos_{ci::vec2(-1, -1)},
    window_center_{getWindowCenter()},
    board_size_{9},
    selected_box_{-1, -1},
    default_text_size_{30},
    is_penciling_{false},
    game_modes_{{"Standard", "Time Attack", "Time Trial"}}
    {}

void MyApp::setup() {
  ci::gl::enableDepthWrite();
  ci::gl::enableDepthRead();

  // Load entry mode images
  ci::gl::Texture2dRef marker_image = ci::gl::Texture2d::create(
      ci::loadImage(loadAsset("marker.png")));
  entry_type_images_.push_back(marker_image);
  ci::gl::Texture2dRef pencil_image = ci::gl::Texture2d::create(
      ci::loadImage(loadAsset("pencil2.png")));
  entry_type_images_.push_back(pencil_image);

  // Record the positions of the menu buttons
  for (size_t i = 0; i < game_modes_.size(); i++) {
    ci::vec2 top_left(window_center_.x - 120,
                      window_center_.y - 120 + i * 90);
    ci::vec2 bottom_right(window_center_.x + 120,
                          window_center_.y - 60 + i * 90);

    std::vector<ci::vec2> button_bounds;
    button_bounds.push_back(top_left);
    button_bounds.push_back(bottom_right);

    menu_buttons_.push_back(button_bounds);
  }

  // Record the positions of the squares of the game board
  float tile_size = std::floor(600 / board_size_);
  float left_bound = window_center_.x - (board_size_ / 2) * tile_size;
  float top_bound = window_center_.y - (board_size_ / 2) * tile_size;

  for (size_t i = 0; i < board_size_; i++) {
    std::vector<std::pair<ci::vec2, ci::vec2>> row;

    for (size_t j = 0; j < board_size_; j++) {
      ci::vec2 top_left(left_bound + j * tile_size,
                        top_bound + i * tile_size);
      ci::vec2 bottom_right(left_bound + (j + 1) * tile_size,
                            top_bound + (i + 1) * tile_size);

      std::pair<ci::vec2, ci::vec2> box_bounds;
      box_bounds.first = top_left;
      box_bounds.second = bottom_right;
      row.push_back(box_bounds);
    }

    game_grid_.push_back(row);
  }

  // Record the positions of other boxes on game screen
  std::vector<ci::vec2> menu_button;
  menu_button.emplace_back(5, 5);
  menu_button.emplace_back(105, 55);
  game_buttons_.push_back(menu_button);

  std::vector<ci::vec2> entry_mode_indicator;
  entry_mode_indicator.emplace_back(window_center_.x - 50,
                                    getWindowSize().y - 100);
  entry_mode_indicator.emplace_back(window_center_.x + 50,
                                        getWindowSize().y);
  game_buttons_.push_back(entry_mode_indicator);
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

  ci::Color color((float) 17 / 256,
                (float) 157 / 256,
                 (float) 164 / 256);

  for (size_t i = 0; i < game_modes_.size(); i++) {
    DrawBox(ci::vec2(window_center_.x - 120,
                             window_center_.y - 120 + i * 90),
        ci::vec2(window_center_.x + 120,
                             window_center_.y - 60 + i * 90),
                    color);
  }
}

void MyApp::PrintGameModes() const {
  ci::Color color = ci::Color((float) 255/256,
                            (float) 94/256,
                             (float) 91/256);
  ci::vec2 button_size(200, 25);

  for (size_t i = 0; i < game_modes_.size(); i++) {
    PrintText(game_modes_[i],
              color,
              button_size,
              ci::vec2(window_center_.x,
                        window_center_.y - 90 + (float) 90 * i),
              default_text_size_);
  }
}

void MyApp::DrawGrid() const {
  float tile_size = std::floor(600 / board_size_);
  ci::Color color = ci::Color::black();

  for (auto row : game_grid_) {
    for (auto col : row) {
      DrawBox(col.first, col.second, color);
    }
  }

  // Make the lines around each 3x3 box thicker
  for (size_t i = 0; i < board_size_ + 1; i+= 3) {
    // Vertical lines
    DrawLine(game_grid_[0][0].first.x + i * tile_size - 1,
                 game_grid_[0][0].first.y,
             game_grid_[0][0].first.x + i * tile_size - 1,
             game_grid_[board_size_ - 1][0].second.y,
             color);
    DrawLine(game_grid_[0][0].first.x + i * tile_size + 1,
                 game_grid_[0][0].first.y,
             game_grid_[0][0].first.x + i * tile_size + 1,
             game_grid_[board_size_ - 1][0].second.y,
             color);

    // Horizontal lines
    DrawLine(game_grid_[0][0].first.x,
         game_grid_[0][0].first.y + i * tile_size - 1,
         game_grid_[0][board_size_ - 1].second.x,
         game_grid_[0][0].first.y + i * tile_size - 1,
         color);
    DrawLine(game_grid_[0][0].first.x,
         game_grid_[0][0].first.y + i * tile_size + 1,
         game_grid_[0][board_size_ - 1].second.x,
         game_grid_[0][0].first.y + i * tile_size + 1,
         color);
  }
}

bool HasValue(std::vector<std::string> values, int value) {
  return std::find(values.begin(), values.end(), std::to_string(value)) != values.end();
}

void MyApp::DrawGameScreen() const {
  // Draw back to menu button
  DrawBox(game_buttons_[0][0],
      game_buttons_[0][1],
      ci::Color((float) 17 / 256,
                    (float) 157 / 256,
                     (float) 164 / 256));
  PrintText("Menu",
      ci::Color((float) 255/256,
                    (float) 94/256,
                     (float) 91/256),
       ci::vec2(95, 50),
       ci::vec2(55, 30),
       default_text_size_);

  // Draw entry mode indicator
  ci::Area box(game_buttons_[1][0], game_buttons_[1][1]);
  if (is_penciling_) {
    ci::gl::draw(entry_type_images_[1], box);
  } else {
    ci::gl::draw(entry_type_images_[0], box);
  }

  DrawGrid();

  PrintBoardEntries();
}

void MyApp::PrintBoardEntries() const {
  // Print pencil marks and board entries
  float tile_size = std::floor(600 / board_size_);

  for (size_t row = 0; row < board_size_; row++) {
    for (size_t col = 0; col < board_size_; col++) {
      if (engine_.GetEntry(row, col) == 0) {
        // Print pencil marks
        for (size_t num = 1; num < board_size_ + 1; num++) {
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

        PrintText(std::to_string(engine_.GetEntry(row, col)),
                  ci::Color::black(),
                  text_size,
                  text_loc,
                  50);
      }
    }
  }
}

void MyApp::keyDown(KeyEvent event) {
  // Erase the current contents of a box
  if (event.getCode() == KeyEvent::KEY_BACKSPACE && selected_box_.first != -1) {
    if (engine_.GetEntry(selected_box_.first, selected_box_.second) == 0) {
      engine_.ClearPencilMarks(selected_box_.first, selected_box_.second);
    } else {
      engine_.SetEntry(selected_box_.first, selected_box_.second, 0);
    }
  }

  if (selected_box_.first != -1) {
    // Update current pencil marks
    if (is_penciling_
        && engine_.GetEntry(selected_box_.first, selected_box_.second) == 0) {
      for (size_t i = 1; i < board_size_ + 1; i++) {
        if (event.getCode() == i + 48) {
          engine_.Pencil(selected_box_.first, selected_box_.second, i);

          break;
        }
      }
    // Update board entries
    } else if (!is_penciling_) {
      for (size_t i = 1; i < board_size_ + 1; i++) {
        if (event.getCode() == i + 48) {
          engine_.SetEntry(selected_box_.first, selected_box_.second, i);

          break;
        }
      }
    }
  }
}

bool IsMouseInBox(const ci::vec2& mouse_pos, std::vector<ci::vec2> box_bounds) {
  return mouse_pos.x > box_bounds[0].x
         && mouse_pos.x < box_bounds[1].x
         && mouse_pos.y > box_bounds[0].y
         && mouse_pos.y < box_bounds[1].y;
}

void MyApp::mouseDown(ci::app::MouseEvent event) {

  if (event.isLeft()) {
    if (state_ == GameState::kMenu) { //combine into for loop
      if (IsMouseInBox(mouse_pos_, menu_buttons_[0])) {
        state_ = GameState::kPlaying;
        // engine.start("Standard);
      } else if (IsMouseInBox(mouse_pos_, menu_buttons_[1])) {
        state_ = GameState::kPlaying;
        // engine.start("Time Attack);
      } else if (IsMouseInBox(mouse_pos_, menu_buttons_[2])) {
        state_ = GameState::kPlaying;
        // engine.start("Time Trial);
      }
    } else if (state_ == GameState::kPlaying) {
      if (IsMouseInBox(mouse_pos_, game_buttons_[0])) {
        state_ = GameState::kMenu;

        ResetGameBoard();
      }

      for (size_t row = 0; row < board_size_; row++) {
        for (size_t col = 0; col < board_size_; col++) {
          std::vector<ci::vec2> bounds{game_grid_[row][col].first, game_grid_[row][col].second};

          if (IsMouseInBox(mouse_pos_, bounds)) {
            selected_box_ = {row, col}; // this feels wrong
          }
        }
      }
    }
  }

  if (event.isRight() && state_ == GameState::kPlaying) {
    is_penciling_ = !is_penciling_;
  }
}
void MyApp::ResetGameBoard() {
  selected_box_ = {-1, -1};

  for (size_t i = 0; i < board_size_ * board_size_; i++) {
    board_entries_[i] = "0";
    board_pencil_marks_[i].clear();
  }
}
}  // namespace myapp
