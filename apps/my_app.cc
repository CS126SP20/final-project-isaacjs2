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

#include <sudoku/engine.h>
#include <sudoku/utils.h>

#include <fstream>

const char kDbPath[] = "leaderboard.db";

namespace myapp {

using cinder::app::KeyEvent;

using Difficulty = sudoku::Engine::Difficulty;
using EntryState = sudoku::Engine::EntryState;
using GameMode = sudoku::Engine::GameMode;

using sudoku::kBoardSize;

using sudoku::DrawBox;
using sudoku::DrawLine;
using sudoku::GetMiddleOfBox;
using sudoku::IsMouseInBox;

const size_t kRegTextSize = 30;
const size_t kBigTextSize = 50;

MyApp::MyApp()
    : state_{AppState::kMenu},
    mouse_pos_{ci::vec2(-1, -1)},
    win_center_{getWindowCenter()},
    sel_box_{-1, -1},
    leaderboard_{cinder::app::getAssetPath(kDbPath).string()},
    want_instructions_{true},
    is_entering_name_{true},
    player_name_{""},
    game_modes_{{"Standard", "Time Trial", "Time Attack"}}
    {}

void MyApp::setup() {
  ci::gl::enableDepthWrite();
  ci::gl::enableDepthRead();

  SetupMenu();
  SetupGameScreen();
  SetupGameOver();
}

void MyApp::update() {
  mouse_pos_ = getMousePos() - getWindowPos();

  if (state_ == AppState::kPlaying) {
    engine_.UpdateGameTime();
  }

  if (engine_.IsGameOver()) {
    engine_.IncreaseGamesCompleted();

    // End the game or give a new board based on the mode and boards completed
    if (engine_.GetGameMode() == GameMode::kStandard) {
      state_ = AppState::kGameOver;
    } else {
      if (engine_.GetGamesCompleted() < 3) {
        if (engine_.GetGameMode() == GameMode::kTimeAttack) {
          engine_.IncreaseDifficulty();
        }

        engine_.CreateGame();
      } else {
        state_ = AppState::kGameOver;
      }
    }
  }

  if (state_ == AppState::kGameOver) {
    UpdateLeaderboard();
  }
}

void MyApp::draw() {
  cinder::gl::enableAlphaBlending();
  cinder::gl::clear(ci::Color((float) 188/256,
                              (float) 188/256,
                              (float) 188/256));

  if (state_ == AppState::kMenu) {
    DrawMenu();
  } else if (state_ == AppState::kPlaying) {
    // Highlight the box that the player has selected
    if (sel_box_.first != -1) {
      HighlightSelectedBox();
    }

    DrawGameScreen();
  } else if (state_ == AppState::kGameOver) {
    DrawGameOver();
  }
}

void MyApp::keyDown(KeyEvent event) {
  // Erase the current contents of a box
  if (event.getCode() == KeyEvent::KEY_BACKSPACE
      && sel_box_.first != -1
      && engine_.GetEntryState(sel_box_)
         != sudoku::Engine::EntryState::kCorrect) {
    if (engine_.GetEntry(sel_box_) == 0) {
      engine_.ClearPencilMarks(sel_box_);
    } else {
      engine_.SetEntry(sel_box_, 0);
    }
  }

  if (sel_box_.first != -1) {
    // The KeyEvent codes for 1-9 are 49-57, hence the offset
    int key_code_offset = 48;

    // If the key pressed is a number, update the board accordingly
    if (event.getCode() > 48 && event.getCode() < 58) {
      // If in pencil mode and if there's no pen mark already in the box, pencil
      if (engine_.IsPenciling() && engine_.GetEntry(sel_box_) == 0) {
        engine_.ChangePencilMark(sel_box_,
                                 event.getCode() - key_code_offset);
      // Fill in box if in pen mode and if the box entry is not yet locked in
      } else if (!engine_.IsPenciling()
                 && engine_.GetEntryState(sel_box_)
                    != sudoku::Engine::EntryState::kCorrect) {
        engine_.SetEntry(sel_box_,
                         event.getCode() - key_code_offset);

        // Change the state of the new entry to unknown
        engine_.ResetEntryState(sel_box_);
      }
    }
  }

  ExecuteArrowKeyMovement(event.getCode());

  if (state_ == AppState::kGameOver && is_entering_name_) {
    UpdatePlayerName(event);
  }
}

void MyApp::mouseDown(ci::app::MouseEvent event) {
  if (event.isLeft()) {
    if (state_ == AppState::kMenu) {
      // Start a game based on the mode clicked
      for (size_t i = 0; i < game_start_btns_.size(); i++) {
        if (IsMouseInBox(mouse_pos_, game_start_btns_[i])) {
          StartNewGame(i);
        }
      }

      // Change difficulty
      if (IsMouseInBox(mouse_pos_, difficulty_btn_)) {
        engine_.IncreaseDifficulty();
      }

      // Toggle instructions
      if (IsMouseInBox(mouse_pos_, instructions_btn_)) {
        want_instructions_ = !want_instructions_;
      }
    } else if (state_ == AppState::kPlaying) {
      // Check if any of the buttons or grid boxes were clicked
      ExecuteGameClick();
    } else if (state_ == AppState::kGameOver && !is_entering_name_) {
      if (IsMouseInBox(mouse_pos_, play_again_btn_)) {
        ResetApp();
      }
    }
  }

  // Toggle between pen and pencil mode
  if (event.isRight() && state_ == AppState::kPlaying) {
    engine_.SwitchEntryMode();
  }
}

void MyApp::SetupMenu() {
  // Record the positions of the game start buttons
  for (size_t i = 0; i < game_modes_.size(); i++) {
    ci::vec2 top_left(win_center_.x - 120,
                      win_center_.y - 120 + i * 90);
    ci::vec2 bottom_right(win_center_.x + 120,
                          win_center_.y - 60 + i * 90);

    std::pair<ci::vec2, ci::vec2> button_bounds(top_left, bottom_right);
    game_start_btns_.push_back(button_bounds);
  }

  // Record the position of the difficulty and instructions buttons
  ci::vec2 diff_button_tl(getWindowBounds().x1 + 10,
                          getWindowBounds().y2 - 60);
  ci::vec2 diff_button_br(getWindowBounds().x1 + 135,
                          getWindowBounds().y2 - 10);
  difficulty_btn_ = {diff_button_tl, diff_button_br};

  ci::vec2 instr_button_tl(getWindowBounds().x2 - 110,
                           getWindowBounds().y2 - 60);
  ci::vec2 instr_button_br(getWindowBounds().x2 - 10,
                           getWindowBounds().y2 - 10);
  instructions_btn_ = {instr_button_tl, instr_button_br};

}

void MyApp::SetupGameScreen() {
  // Load entry mode images
  ci::gl::Texture2dRef marker_image = ci::gl::Texture2d::create(
      ci::loadImage(loadAsset("marker.png")));
  entry_type_images_[0] = marker_image;

  ci::gl::Texture2dRef pencil_image = ci::gl::Texture2d::create(
      ci::loadImage(loadAsset(
                                          "pencil2.png")));
  entry_type_images_[1] = pencil_image;

  SetupGameBoard();

  // Record the positions of buttons and boxes on game screen
  menu_return_btn_.first = {5, 5};
  menu_return_btn_.second = {105, 55};

  hint_btn_.first = {game_grid_[0][0].first.x - 100,
                     game_grid_[kBoardSize - 1][0].second.y - 105};
  hint_btn_.second = {game_grid_[0][0].first.x - 5,
                      game_grid_[kBoardSize - 1][0].second.y - 55};

  check_board_btn_.first = {game_grid_[0][0].first.x - 100,
                            game_grid_[kBoardSize - 1][0].second.y - 50};
  check_board_btn_.second = {game_grid_[0][0].first.x - 5,
                            game_grid_[kBoardSize - 1][0].second.y};

  entry_mode_indicator_.first = {win_center_.x - 50,
                                 getWindowSize().y - 100};
  entry_mode_indicator_.second = {win_center_.x + 50,
                                      getWindowSize().y};
}

void MyApp::SetupGameBoard() {
  float tile_size = std::floor(600 / kBoardSize);
  float left_bound = win_center_.x - ((float) kBoardSize / 2) * tile_size;
  float top_bound = win_center_.y - ((float) kBoardSize / 2) * tile_size;

  // Record the position of each box of the grid
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

void MyApp::SetupGameOver() {
  // Record position of play again button
  play_again_btn_.first = {win_center_.x - 50,
                          getWindowBounds().y2 - 55};
  play_again_btn_.second = {win_center_.x + 50,
                           getWindowBounds().y2 - 5};
}

void MyApp::UpdateLeaderboard() {
  if (top_players_.empty() && !is_entering_name_) {
    std::string mode = GetModeAsString();
    std::string difficulty = GetDifficultyAsString();

    // This mode goes through all the difficulties, so I standardize it here
    if (engine_.GetGameMode() == GameMode::kTimeAttack) {
      difficulty = "Easy";
    }

    leaderboard_.AddTimeToLeaderBoard({player_name_,
                                       static_cast<size_t>(
                                           engine_.GetGameTime())},
                                      mode,
                                      difficulty);

    // Update the list of top players in case the newest score is on it
    top_players_ = leaderboard_.RetrieveBestTimes(10, mode, difficulty);
  }
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

  if (want_instructions_) {
    PrintMenuInstructions();
  }
}

void MyApp::PrintGameModes() const {
  for (size_t i = 0; i < game_modes_.size(); i++) {
    PrintText(game_modes_[i],
              ci::Color(1, 0, 0),
              ci::vec2(200, 25),
              ci::vec2(GetMiddleOfBox(game_start_btns_[i])),
              kRegTextSize);
  }
}

void MyApp::DrawSettings() const {
  // Draw difficulty picker
  ci::Color diff_color;
  if (engine_.GetDifficulty() == Difficulty::kEasy) {
    diff_color = ci::Color(0, 1, 0);
  } else if (engine_.GetDifficulty() == Difficulty::kMedium) {
    diff_color = ci::Color(1, 1, 0);
  } else if (engine_.GetDifficulty() == Difficulty::kHard) {
    diff_color = ci::Color(1, 0, 0);
  }

  std::string difficulty = GetDifficultyAsString();

  PrintText("Difficulty",
            ci::Color::black(),
            ci::vec2(200, 35),
            ci::vec2(difficulty_btn_.first.x + 55,
                         difficulty_btn_.first.y - 20),
            40);
  PrintText(difficulty,
            diff_color,
            ci::vec2(120, 40),
            ci::vec2(GetMiddleOfBox(difficulty_btn_)),
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
            ci::vec2(GetMiddleOfBox(instructions_btn_)),
            40);
  PrintText("Instructions",
            ci::Color::black(),
            ci::vec2(200, 35),
            ci::vec2(instructions_btn_.second.x - 75,
                     instructions_btn_.first.y - 20),
            40);
  DrawBox(instructions_btn_, ci::Color::black());
}

void MyApp::DrawGameScreen() {
  // Draw timer
  PrintText("Time", ci::Color::black(),
            ci::vec2(100, 40),
            ci::vec2(game_grid_[0][kBoardSize - 1].second.x + 55,
                     game_grid_[0][kBoardSize - 1].first.y + 20),
            40);
  PrintText(std::to_string(engine_.GetGameTime()),
            ci::Color::black(),
            ci::vec2(100, 30),
            ci::vec2(game_grid_[0][kBoardSize - 1].second.x + 55,
                     game_grid_[0][kBoardSize - 1].first.y + 60),
            kRegTextSize);

  // Draw entry mode indicator
  ci::Area box(entry_mode_indicator_.first,
               entry_mode_indicator_.second);
  if (engine_.IsPenciling()) {
    ci::gl::draw(entry_type_images_[1], box);
  } else {
    ci::gl::draw(entry_type_images_[0], box);
  }

  DrawGameButtons();

  DrawGrid();

  PrintBoardEntries();

  if (want_instructions_) {
    PrintGameInstructions();
  }
}

void MyApp::DrawGameButtons() {
  // Draw back to menu button
  DrawBox(menu_return_btn_, ci::Color(0, 0, 1));
  PrintText("Menu",
            ci::Color(1, 0, 0),
            ci::vec2(95, 50),
            ci::vec2(GetMiddleOfBox(menu_return_btn_)),
            kRegTextSize);

  // Draw hint button
  DrawBox(hint_btn_, ci::Color(0, 0, 1));
  PrintText("Hint",
            ci::Color(1, 0, 0),
            ci::vec2(95, 45),
            GetMiddleOfBox(hint_btn_),
            kRegTextSize);

  // Draw check board button
  DrawBox(check_board_btn_, ci::Color(0, 0, 1));
  PrintText("Check Board",
            ci::Color(1, 0, 0),
            ci::vec2(95, 45),
            GetMiddleOfBox(check_board_btn_),
            kRegTextSize);
}

void MyApp::DrawGrid() const {
  float tile_size = std::floor(600 / kBoardSize);

  ci::Color color = ci::Color::black();

  // Draw each box of the grid
  for (const auto& row : game_grid_) {
    for (const auto& col : row) {
      DrawBox(col, color);
    }
  }

  // Make the lines around each 3x3 box thicker
  for (size_t i = 0; i < kBoardSize + 1; i+= 3) {
    // Vertical lines
    DrawLine( game_grid_[0][0].first.x + i * tile_size - 1,
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

void MyApp::PrintBoardEntries() const {
  float tile_size = std::floor(600 / kBoardSize);

  // Print pencil marks and board entries
  for (size_t row = 0; row < kBoardSize; row++) {
    for (size_t col = 0; col < kBoardSize; col++) {
      if (engine_.GetEntry({row, col}) == 0) {
        // Print pencil marks if no regular entry
        for (size_t num = 1; num < kBoardSize + 1; num++) {
          if (engine_.IsPenciled({row, col}, num)) {
            // Put numbers in a mini 3x3 grid in the box
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
        // Print regular board entries
        ci::vec2 text_size(35, 35);
        ci::vec2 text_loc(GetMiddleOfBox(game_grid_[row][col]));

        // Change the color of the number based on its state
        ci::Color color(ci::Color::black());
        switch (engine_.GetEntryState({row, col})) {
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

        PrintText(std::to_string(engine_.GetEntry({row, col})),
                  color,
                  text_size,
                  GetMiddleOfBox(game_grid_[row][col]),
                  kBigTextSize);
      }
    }
  }
}

void MyApp::HighlightSelectedBox() const {
  ci::Color color(1, 0, 0);
  DrawBox(game_grid_[sel_box_.first][sel_box_.second], color);

  // Make the border thicker
  DrawBox({
              ci::vec2(
                  game_grid_[sel_box_.first][sel_box_.second].first.x - 1,
                  game_grid_[sel_box_.first][sel_box_.second].first.y - 1),
              ci::vec2(
                  game_grid_[sel_box_.first][sel_box_.second].second.x + 1,
                  game_grid_[sel_box_.first][sel_box_.second].second.y
                      + 1)},
          color);
  DrawBox({
              ci::vec2(
                  game_grid_[sel_box_.first][sel_box_.second].first.x + 1,
                  game_grid_[sel_box_.first][sel_box_.second].first.y + 1),
              ci::vec2(
                  game_grid_[sel_box_.first][sel_box_.second].second.x - 1,
                  game_grid_[sel_box_.first][sel_box_.second].second.y
                      - 1)},
          color);
}

void MyApp::DrawGameOver() const {
  PrintText("You Win! \n Time: "
                  + std::to_string(engine_.GetGameTime())
                  + " seconds",
            ci::Color(0, 0, 1),
            ci::vec2(700, 120),
            ci::vec2(win_center_.x, getWindowBounds().y1 + 60),
            60);

  // Print the type that the game was
  std::string game_type;
  if (engine_.GetGameMode() == GameMode::kTimeAttack) {
      game_type = GetModeAsString();
  } else {
      game_type = GetDifficultyAsString() + " " + GetModeAsString();
  }
  PrintText("Mode: " + game_type,
           ci::Color(0, 0, 1),
           ci::vec2(700, 60),
           ci::vec2(win_center_.x, getWindowBounds().y1 + 150),
           60);

  if (is_entering_name_) {
    if (want_instructions_) {
      PrintEnterNameInstructions();
    }

    PrintText("Enter name:",
              ci::Color::black(),
              ci::vec2(250, 50),
              ci::vec2(win_center_.x, win_center_.y - 50),
              kBigTextSize);
    PrintText(player_name_,
              ci::Color::black(),
              ci::vec2(600, 50),
              win_center_,
              kBigTextSize);
  } else {
    DrawLeaderboard();

    // Draw play again button
    DrawBox(play_again_btn_,ci::Color(1, 0, 0));
    PrintText("Play Again",
              ci::Color(0, 0, 1),
              ci::vec2(95, 45),
              ci::vec2(GetMiddleOfBox(play_again_btn_)),
              kRegTextSize);
  }
}

void MyApp::DrawLeaderboard() const {
  PrintText("Player",
            ci::Color::black(),
            ci::vec2(300, 50),
            ci::vec2(win_center_.x - 50, win_center_.y - 190),
            kBigTextSize);
  PrintText("Time",
            ci::Color::black(),
            ci::vec2(300, 50),
            ci::vec2(win_center_.x + 250, win_center_.y - 190),
            kBigTextSize);

  //Print top 10 scores
  ci::Color color;
  for (size_t i = 0; i < top_players_.size(); i++) {
    // Highlight player's scores if on the leaderboard
    if (top_players_[i].name == player_name_) {
      color = ci::Color(1, 1, 0);
    } else {
      color = ci::Color::black();
    }

    PrintText(std::to_string(i + 1),
              color,
              ci::vec2(100, 50),
              ci::vec2(getWindowBounds().x1 + 125,
                           win_center_.y - 140 + i * 50),
              kBigTextSize);

    PrintText(top_players_[i].name,
              color,
              ci::vec2(300, 50),
              ci::vec2(win_center_.x - 50,
                           win_center_.y - 140 + i * 50),
              kBigTextSize);

    PrintText(std::to_string(top_players_[i].time),
              color,
              ci::vec2(300, 50),
              ci::vec2(win_center_.x + 250,
                           win_center_.y - 140 + i * 50),
              kBigTextSize);
  }
}

void MyApp::ExecuteGameClick() {
  if (IsMouseInBox(mouse_pos_, menu_return_btn_)) {
    ResetApp();
  }

  // Give a hint
  if (IsMouseInBox(mouse_pos_, hint_btn_) && sel_box_.first != -1) {
    engine_.FillInCorrectEntry(sel_box_);
  }

  if (IsMouseInBox(mouse_pos_, check_board_btn_)) {
    engine_.CheckBoard();
  }

  // Updates the last box in the board the player clicked on
  for (size_t row = 0; row < kBoardSize; row++) {
    for (size_t col = 0; col < kBoardSize; col++) {
      if (IsMouseInBox(mouse_pos_, game_grid_[row][col])) {
        sel_box_ = {row, col};
      }
    }
  }
}

void MyApp::ExecuteArrowKeyMovement(int key_code) {
  if (sel_box_.first != -1) {
    // Navigate board with arrow keys
    if (key_code == KeyEvent::KEY_UP && sel_box_.first > 0) {
      sel_box_.first--;
    } else if (key_code == KeyEvent::KEY_DOWN && sel_box_.first < 8) {
      sel_box_.first++;
    } else if (key_code == KeyEvent::KEY_LEFT && sel_box_.second > 0) {
      sel_box_.second--;
    } else if (key_code == KeyEvent::KEY_RIGHT && sel_box_.second < 8) {
      sel_box_.second++;
    }
  }

  // If an arrow key is pressed and no box is selected, select the middle box
  if (state_ == AppState::kPlaying && sel_box_.first == -1) {
    if (key_code == KeyEvent::KEY_UP
        || key_code == KeyEvent::KEY_DOWN
        || key_code == KeyEvent::KEY_LEFT
        || key_code == KeyEvent::KEY_RIGHT) {
      sel_box_ = {4, 4};
    }
  }
}

void MyApp::UpdatePlayerName(KeyEvent event) {
  int max_name_len = 10;

  // The bounds on the event code make sure the key pressed has a char on it
  // For example, letters and symbols are valid but F1 is not
  if (player_name_.length() < max_name_len
      && event.getCode() > 32
      && event.getCode() < 123) {
    player_name_ = player_name_ + event.getChar();
  }

  if (event.getCode() == KeyEvent::KEY_RETURN) {
    // If no name is entered, switch to the default
    if (player_name_.length() == 0) {
      player_name_ = "Anonymous";
    }

    // This lets the game know to load the game over/leaderboard screen
    is_entering_name_ = false;
  }

  // Backspace functionality
  if (event.getCode() == KeyEvent::KEY_BACKSPACE && player_name_.length() > 0) {
    player_name_ = player_name_.substr(0,
                                     player_name_.length() - 1);
  }
}

void MyApp::StartNewGame(int mode) {
  switch (mode) {
    case 0:
      engine_.SetGameMode(GameMode::kStandard);
      break;
    case 1:
      engine_.SetGameMode(GameMode::kTimeTrial);
      break;
    case 2:
      engine_.SetGameMode(GameMode::kTimeAttack);
      engine_.SetDifficulty(Difficulty::kEasy);
      break;
  }

  state_ = AppState::kPlaying;
  engine_.CreateGame();
  engine_.SetStartTime(std::chrono::system_clock::now());
}

void MyApp::ResetApp() {
  state_ = AppState::kMenu;
  engine_.ResetGame();
  top_players_.clear();
  sel_box_ = {-1, -1};

  is_entering_name_ = true;
  player_name_ = "";
}

void MyApp::PrintMenuInstructions() const {
  // Print game mode explanations
  PrintText("Classic game of the desired difficulty",
            ci::Color::black(),
            ci::vec2(250, 60),
            ci::vec2(win_center_.x - 250,
                     GetMiddleOfBox(game_start_btns_[0]).y),
            kRegTextSize);
  PrintText("Three games in a row of one difficutly",
            ci::Color::black(),
            ci::vec2(250, 60),
            ci::vec2(win_center_.x - 250,
                     GetMiddleOfBox(game_start_btns_[1]).y),
            kRegTextSize);
  PrintText("Three games in a row going from Easy to Hard",
            ci::Color::black(),
            ci::vec2(270, 60),
            ci::vec2(win_center_.x - 260,
                     GetMiddleOfBox(game_start_btns_[2]).y),
            kRegTextSize);

  // Print settings instructions
  PrintText("Choose your difficulty before starting a game",
            ci::Color::black(),
            ci::vec2(250, 60),
            ci::vec2(GetMiddleOfBox(difficulty_btn_).x + 180,
                     GetMiddleOfBox(difficulty_btn_).y),
            kRegTextSize);
  PrintText("These go away when you turn off instructions",
            ci::Color::black(),
            ci::vec2(250, 60),
            ci::vec2(GetMiddleOfBox(instructions_btn_).x - 180,
                     GetMiddleOfBox(instructions_btn_).y),
            kRegTextSize);
}

void MyApp::PrintGameInstructions() const {
  PrintText("Welcome to Sudoku! To solve the puzzle,",
            ci::Color::black(),
            ci::vec2(600, 30),
            ci::vec2(win_center_.x, getWindowBounds().y1 + 20),
            kRegTextSize);
  PrintText("fill every row, column, and box with the numbers 1 to 9.",
            ci::Color::black(),
            ci::vec2(600, 30),
            ci::vec2(win_center_.x, getWindowBounds().y1 + 50),
            kRegTextSize);
  PrintText("Use your mouse and number pad to fill in the board.",
            ci::Color::black(),
            ci::vec2(600, 30),
            ci::vec2(win_center_.x, getWindowBounds().y1 + 80),
            kRegTextSize);

  PrintText("This symbol shows what entry mode you're in.",
            ci::Color::black(),
            ci::vec2(350, 20),
            ci::vec2(620,
                         game_grid_[kBoardSize - 1][0].second.y + 10),
            20);
  PrintText("Pen mode is for filling in the board, and",
            ci::Color::black(),
            ci::vec2(350, 20),
            ci::vec2(620,
                         game_grid_[kBoardSize - 1][0].second.y + 30),
            20);
  PrintText("pencil mode is for making notes when you're",
            ci::Color::black(),
            ci::vec2(350, 20),
            ci::vec2(620,
                         game_grid_[kBoardSize - 1][0].second.y + 50),
            20);
  PrintText("not sure what number goes in a box yet.",
            ci::Color::black(),
            ci::vec2(350, 20),
            ci::vec2(620,
                         game_grid_[kBoardSize - 1][0].second.y + 70),
            20);

  PrintText("Click 'Check Board' lock in correct entries and",
            ci::Color::black(),
            ci::vec2(350, 20),
            ci::vec2(175,
                         game_grid_[kBoardSize - 1][0].second.y + 10),
            20);
  PrintText("highlight wrong ones. When you think you have",
            ci::Color::black(),
            ci::vec2(350, 20),
            ci::vec2(175,
                         game_grid_[kBoardSize - 1][0].second.y + 30),
            20);
  PrintText("the solution, click it one more time to end the",
            ci::Color::black(),
            ci::vec2(350, 20),
            ci::vec2(175,
                         game_grid_[kBoardSize - 1][0].second.y + 50),
            20);
  PrintText("game. Click 'Hint' to fill in the box you have",
            ci::Color::black(),
            ci::vec2(350, 20),
            ci::vec2(175,
                         game_grid_[kBoardSize - 1][0].second.y + 70),
            20);
  PrintText("highlighted with the correct number.",
            ci::Color::black(),
            ci::vec2(350, 20),
            ci::vec2(175,
                         game_grid_[kBoardSize - 1][0].second.y + 90),
            20);
}

void MyApp::PrintEnterNameInstructions() const {
  PrintText("Enter a name between 1 and 10 characters "
                 "and hit enter to submit it",
            ci::Color::black(),
            ci::vec2(500, 60),
            ci::vec2(win_center_.x, win_center_.y + 300),
            kRegTextSize);
}

std::string MyApp::GetModeAsString() const {
  std::string mode;
  if (engine_.GetGameMode() == GameMode::kStandard) {
    mode = "Standard";
  } else if (engine_.GetGameMode() == GameMode::kTimeTrial) {
    mode = "Time Trial";
  } else if (engine_.GetGameMode() == GameMode::kTimeAttack) {
    mode = "Time Attack";
  }

  return mode;
}

std::string MyApp::GetDifficultyAsString() const {
  std::string difficulty;
  if (engine_.GetDifficulty() == Difficulty::kEasy) {
    difficulty = "Easy";
  } else if (engine_.GetDifficulty() == Difficulty::kMedium) {
    difficulty = "Medium";
  } else if (engine_.GetDifficulty() == Difficulty::kHard) {
    difficulty = "Hard";
  }

  return difficulty;
}

}  // namespace myapp
