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
  // Record positions of buttons in the menu
  void SetupMenu();

  // Record positions of buttons and game board boxes in the game
  void SetupGameScreen();
  void SetupGameBoard();

  // Record the positions of buttons in the game over screen
  void SetupGameOver();

  // Add the player's time to the leaderboard and get the new top 10 times
  void UpdateLeaderboard();

  // Draw the parts of the menu
  void DrawMenu() const;
  void PrintGameModes() const;
  void DrawSettings() const;

  // Draw the parts of the game screen
  void DrawGameScreen();
  void DrawGameButtons();
  void DrawGrid() const;
  void PrintBoardEntries() const;
  void HighlightSelectedBox() const;

  // Draw parts of the game over screen
  void DrawGameOver() const;
  void DrawLeaderboard() const;

  // Check if the player clicked anything in the game screen and update the
  // game accordingly
  void ExecuteGameClick();

  // Update the highlighted box on the board if the player pressed an arrow key
  void ExecuteArrowKeyMovement(int key_code);

  // Add to, delete from, or confirm the player's name at the end of the game
  void UpdatePlayerName(cinder::app::KeyEvent event);



  // Get a new game board from the engine and switch to the game screen
  // For the mode parameter, 0 = Standard, 1 = Time Trial, and 2 = Time Attack
  void StartNewGame(int mode);

  // Reset instance variables and return to the menu
  void ResetApp();

  // Print instructions for different parts of the app
  void PrintMenuInstructions() const;
  void PrintGameInstructions() const;
  void PrintEnterNameInstructions() const;

  // Translate game characteristics into strings for printing
  string GetModeAsString() const;
  string GetDifficultyAsString() const;

  // The state of the app indicates what screen it is on
  GameState state_;

  // x,y position of the mouse relative to the window
  ci::vec2 mouse_pos_;

  // x,y point at the center of the window
  ci::vec2 win_center_;

  // Row,Column coordinate of the selected box in the game board
  // If not in a game, the coordinates are (-1, -1)
  pair<int, int> sel_box_;

  sudoku::Engine engine_;
  sudoku::LeaderBoard leaderboard_;

  // Top players and their times, updated based on game's mode/difficulty
  vector<sudoku::Player> top_players_;

  // Whether or not to print the instructions for each screen
  bool want_instructions_;

  // For the game over screen, indicates if the player has confirmed their name
  bool is_entering_name_;
  string player_name_;

  // Each pair indicates the positions of the top left and bottom right of the
  // button or box
  vector<pair<ci::vec2, ci::vec2>> game_start_btns_;
  pair<ci::vec2, ci::vec2> difficulty_btn_;
  pair<ci::vec2, ci::vec2> instructions_btn_;
  pair<ci::vec2, ci::vec2> menu_return_btn_;
  pair<ci::vec2, ci::vec2> hint_btn_;
  pair<ci::vec2, ci::vec2> check_board_btn_;
  pair<ci::vec2, ci::vec2> play_again_btn_;
  pair<ci::vec2, ci::vec2> entry_mode_indicator_;
  array<array<pair<ci::vec2, ci::vec2>, kBoardSize>, kBoardSize> game_grid_;

  // Names of the game modes
  vector<string> game_modes_;

  // Pen and pencil images
  array<ci::gl::Texture2dRef, 2> entry_type_images_;
};

}  // namespace myapp

#endif  // FINALPROJECT_APPS_MYAPP_H_
