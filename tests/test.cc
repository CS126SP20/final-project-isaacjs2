// Copyright (c) 2020 CS126SP20. All rights reserved.

#define CATCH_CONFIG_MAIN

#include <cinder/Vector.h>

#include <sudoku/engine.h>
#include <sudoku/utils.h>

#include <catch2/catch.hpp>

using Difficulty = sudoku::Engine::Difficulty;
using EntryState = sudoku::Engine::EntryState;
using GameMode = sudoku::Engine::GameMode;
using sudoku::kBoardSize;

using sudoku::GetMiddleOfBox;
using sudoku::IsMouseInBox;

TEST_CASE("Get Middle of Box", "[util]") {
  std::pair<ci::vec2, ci::vec2> box_bounds = {{10, 10},
                                              {50, 90}};

  REQUIRE(GetMiddleOfBox(box_bounds) == ci::vec2(30, 50));
}

TEST_CASE("Check if mouse is in a given box", "[utils]") {
  ci::vec2 mouse_pos(100, 100);
  std::pair<ci::vec2, ci::vec2> box_bounds;

  SECTION("In the box") {
    box_bounds = {{0, 0}, {200, 200}};

    REQUIRE(IsMouseInBox(mouse_pos, box_bounds));
  }

  SECTION("Not in the box") {
    box_bounds = {{200, 200}, {300, 300}};

    REQUIRE(!IsMouseInBox(mouse_pos, box_bounds));
  }
}

TEST_CASE("Check if a number is penciled in a box", "[engine][pencil]") {
  sudoku::Engine engine;
  engine.CreateGame("test_board.json");

  SECTION("Not penciled") {
    REQUIRE(!engine.IsPenciled({0, 0}, 1));
  }

  SECTION("Penciled") {
    engine.ChangePencilMark({0, 0}, 1);

    REQUIRE(engine.IsPenciled({0, 0}, 1));
  }
}

TEST_CASE("Change pencil marks", "[engine][pencil]") {
  sudoku::Engine engine;
  engine.CreateGame("test_board.json");

  SECTION("Penciled") {
    engine.ChangePencilMark({0, 0}, 1);

    REQUIRE(engine.IsPenciled({0, 0}, 1));
  }

  SECTION("Undo pencil mark") {
    engine.ChangePencilMark({0, 0}, 1);
    engine.ChangePencilMark({0, 0}, 1);

    REQUIRE(!engine.IsPenciled({0, 0}, 1));
  }
}

TEST_CASE("Clear pencil marks", "[engine][pencil]") {
  sudoku::Engine engine;
  engine.CreateGame("test_board.json");

  SECTION("No pencil marks to clear") {
    engine.ClearPencilMarks({0, 0});

    bool cleared = true;
    for (size_t num = 1; num <= kBoardSize; num++) {
      if (engine.IsPenciled({0, 0}, num)) {
        cleared = false;
      }
    }

    REQUIRE(cleared);
  }

  SECTION("Clear all marks") {
    for (size_t num = 1; num <= kBoardSize; num++) {
      engine.ChangePencilMark({0, 0}, num);
    }

    engine.ClearPencilMarks({0, 0});

    bool cleared = true;
    for (size_t num = 1; num <= kBoardSize; num++) {
      if (engine.IsPenciled({0, 0}, num)) {
        cleared = false;
      }
    }

    REQUIRE(cleared);
  }
}

TEST_CASE("Increase difficulty", "[engine]") {
  sudoku::Engine engine;

  SECTION("Normal increase") {
    engine.SetDifficulty(Difficulty::kEasy);
    engine.IncreaseDifficulty();

    REQUIRE(engine.GetDifficulty() == Difficulty::kMedium);
  }

  SECTION("Increase when at max difficulty") {
    engine.SetDifficulty(Difficulty::kHard);
    engine.IncreaseDifficulty();

    REQUIRE(engine.GetDifficulty() == Difficulty::kEasy);
  }
}

TEST_CASE("Fill in correct entry", "[engine]") {
  sudoku::Engine engine;
  engine.CreateGame("test_board.json");

  SECTION("Fill in empty box") {
    engine.FillInCorrectEntry({0, 0});

    REQUIRE(engine.GetEntry({0, 0}) == 6);
  }

  SECTION("Fill in box with number already in it") {
    engine.SetEntry({0, 0}, 1);

    engine.FillInCorrectEntry({0, 0});

    REQUIRE(engine.GetEntry({0, 0}) == 6);
  }

  SECTION("Fill in box with pencil marks in it") {
    engine.ChangePencilMark({0, 0}, 1);

    engine.FillInCorrectEntry({0, 0});

    REQUIRE(engine.GetEntry({0, 0}) == 6);
  }

  SECTION("Fill in box with correct number already in it") {
    engine.SetEntry({0, 0}, 6);
    engine.CheckBoard();

    engine.FillInCorrectEntry({0, 0});

    REQUIRE(engine.GetEntry({0, 0}) == 6);
  }
}

TEST_CASE("Check board", "[engine]") {
  sudoku::Engine engine;
  engine.CreateGame("test_board.json");

  SECTION("Empty entry") {
    engine.CheckBoard();

    REQUIRE(engine.GetEntryState({0, 0}) == EntryState::kUnknown);
  }

  SECTION("Wrong entry") {
    engine.SetEntry({0, 0}, 1);

    engine.CheckBoard();

    REQUIRE(engine.GetEntryState({0, 0}) == EntryState::kWrong);
  }

  SECTION("Correct entry") {
    engine.SetEntry({0, 0}, 6);

    engine.CheckBoard();

    REQUIRE(engine.GetEntryState({0, 0}) == EntryState::kCorrect);
  }
}

TEST_CASE("Check if game is over", "[engine]") {
  sudoku::Engine engine;
  engine.CreateGame("test_board.json");

  SECTION("Empty board") {
    REQUIRE(!engine.IsGameOver());
  }

  SECTION("Partially filled board") {
    engine.FillInCorrectEntry({0, 0});

    REQUIRE(!engine.IsGameOver());
  }

  SECTION("Solved board") {
    for (size_t row = 0; row < kBoardSize; row++) {
      for (size_t col = 0; col < kBoardSize; col++) {
        engine.FillInCorrectEntry({row, col});
      }
    }

    REQUIRE(engine.IsGameOver());
  }
}