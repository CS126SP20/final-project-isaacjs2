// Copyright (c) 2020 [Your Name]. All rights reserved.

#include "my_app.h"

#include <cinder/Font.h>
#include <cinder/Text.h>
#include <cinder/Vector.h>
#include <cinder/gl/draw.h>
#include <cinder/gl/gl.h>
#include <cinder/app/App.h>
#include <cinder/Path2d.h>
#include <cinder/Shape2d.h>
#include <cinder/ImageIo.h>
#include <cinder/gl/Texture.h>

namespace myapp {

using cinder::app::KeyEvent;

MyApp::MyApp()
    : state_{GameState::kMenu},
    mouse_pos_{ci::vec2(-1, -1)},
    window_center_{getWindowCenter()},
    board_size_{9},
    selected_box_{-1},
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

  // Fill vectors with empty values so I can access by index later
  for (size_t i = 0; i < board_size_ * board_size_; i++) {
    board_entries_.emplace_back("0");

    std::vector<std::string> marks;
    board_pencil_marks_.push_back(marks);
  }

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

  for (size_t row = 0; row < board_size_; row++) {
    for (size_t col = 0; col < board_size_; col++) {
      ci::vec2 top_left(left_bound + col * tile_size,
                        top_bound + row * tile_size);
      ci::vec2 bottom_right(left_bound + (col + 1) * tile_size,
                            top_bound + (row + 1) * tile_size);

      std::vector<ci::vec2> box_bounds;
      box_bounds.push_back(top_left);
      box_bounds.push_back(bottom_right);
      game_grid_.push_back(box_bounds);
    }
  }

  // Record the positions of extra boxes on game screen
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
    if (selected_box_ != -1) {
      ci::Color color(1, 0, 0);
      DrawBox(game_grid_[selected_box_][0],
          game_grid_[selected_box_][1],
                      color);
      DrawBox(ci::vec2(game_grid_[selected_box_][0].x - 1,
                               game_grid_[selected_box_][0].y - 1),
          ci::vec2(game_grid_[selected_box_][1].x + 1,
                               game_grid_[selected_box_][1].y + 1),
                      color);
      DrawBox(ci::vec2(game_grid_[selected_box_][0].x + 1,
                               game_grid_[selected_box_][0].y + 1),
          ci::vec2(game_grid_[selected_box_][1].x - 1,
                               game_grid_[selected_box_][1].y - 1),
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

  for (std::vector<ci::vec2> box : game_grid_) {
    DrawBox(box[0], box[1], color);
  }

  // Make the lines around each 3x3 box thicker
  for (size_t i = 0; i < board_size_ + 1; i+= 3) {
    DrawLine(game_grid_[0][0].x + i * tile_size - 1,
             game_grid_[0][0].y,
             game_grid_[0][0].x + i * tile_size - 1,
             game_grid_[board_size_ * board_size_ - 1][1].y,
             color);
    DrawLine(game_grid_[0][0].x + i * tile_size + 1,
             game_grid_[0][0].y,
             game_grid_[0][0].x + i * tile_size + 1,
             game_grid_[board_size_ * board_size_ - 1][1].y,
             color);

    DrawLine(game_grid_[0][0].x,
         game_grid_[0][0].y + i * tile_size - 1,
         game_grid_[board_size_ - 1][1].x,
         game_grid_[0][0].y + i * tile_size - 1,
         color);
    DrawLine(game_grid_[0][0].x,
         game_grid_[0][0].y + i * tile_size + 1,
         game_grid_[board_size_ - 1][1].x,
         game_grid_[0][0].y + i * tile_size + 1,
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

  for (size_t i = 0; i < board_entries_.size(); i++) {
    if (board_entries_[i] == "0") {
      // Print pencil marks
      for (size_t j = 1; j < board_size_ + 1; j++) {
        if (HasValue(board_pencil_marks_[i], j)) {
          ci::vec2 mark_loc(game_grid_[i][0].x
                            + tile_size / 6
                            + ((j- 1) % 3) * tile_size / 3,
                            game_grid_[i][0].y
                            + tile_size / 6
                            + ((j - 1) / 3) * tile_size / 3);

          PrintText(std::to_string(j),
                    ci::Color::black(),
                    ci::vec2(tile_size / 3, tile_size / 3),
                    mark_loc,
                    tile_size / 3);
        }
      }
    } else {
      // Print board entries
      ci::vec2 text_size(35, 35);
      ci::vec2 text_loc(game_grid_[i][0].x + tile_size / 2,
                        game_grid_[i][0].y + tile_size / 2);

      PrintText(board_entries_[i],
                ci::Color::black(),
                text_size,
                text_loc,
                50);
    }
  }
}

void MyApp::keyDown(KeyEvent event) {
  if (event.KEY_g) {
    state_ = GameState::kPlaying;
  }

  if (selected_box_ != -1) {
    if (is_penciling_) {
      switch (event.getCode()) {
        case event.KEY_1:
          if (!HasValue(board_pencil_marks_[selected_box_], 1)) {
            board_pencil_marks_[selected_box_].emplace_back("1");
          }
          break;
        case event.KEY_2:
          if (!HasValue(board_pencil_marks_[selected_box_], 2)) {
            board_pencil_marks_[selected_box_].emplace_back("2");
          }
          break;
        case event.KEY_3:
          if (!HasValue(board_pencil_marks_[selected_box_], 3)) {
            board_pencil_marks_[selected_box_].emplace_back("3");
          }
          break;
        case event.KEY_4:
          if (!HasValue(board_pencil_marks_[selected_box_], 4)) {
            board_pencil_marks_[selected_box_].emplace_back("4");
          }
          break;
        case event.KEY_5:
          if (!HasValue(board_pencil_marks_[selected_box_], 5)) {
            board_pencil_marks_[selected_box_].emplace_back("5");
          }
          break;
        case event.KEY_6:
          if (!HasValue(board_pencil_marks_[selected_box_], 6)) {
            board_pencil_marks_[selected_box_].emplace_back("6");
          }
          break;
        case event.KEY_7:
          if (!HasValue(board_pencil_marks_[selected_box_], 7)) {
            board_pencil_marks_[selected_box_].emplace_back("7");
          }
          break;
        case event.KEY_8:
          if (!HasValue(board_pencil_marks_[selected_box_], 8)) {
            board_pencil_marks_[selected_box_].emplace_back("8");
          }
          break;
        case event.KEY_9:
          if (!HasValue(board_pencil_marks_[selected_box_], 9)) {
            board_pencil_marks_[selected_box_].emplace_back("9");
          }
          break;
      }
    } else {
      switch (event.getCode()) {
        case event.KEY_1:
          board_entries_[selected_box_] = "1";
          break;
        case event.KEY_2:
          board_entries_[selected_box_] = "2";
          break;
        case event.KEY_3:
          board_entries_[selected_box_] = "3";
          break;
        case event.KEY_4:
          board_entries_[selected_box_] = "4";
          break;
        case event.KEY_5:
          board_entries_[selected_box_] = "5";
          break;
        case event.KEY_6:
          board_entries_[selected_box_] = "6";
          break;
        case event.KEY_7:
          board_entries_[selected_box_] = "7";
          break;
        case event.KEY_8:
          board_entries_[selected_box_] = "8";
          break;
        case event.KEY_9:
          board_entries_[selected_box_] = "9";
          break;
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
      }

      for (size_t i = 0; i < game_grid_.size(); i++) {
        if (IsMouseInBox(mouse_pos_, game_grid_[i])) {
          selected_box_ = i;
        }
      }
    }
  }

  if (event.isRight() && state_ == GameState::kPlaying) {
    is_penciling_ = !is_penciling_;
  }
}
}  // namespace myapp
