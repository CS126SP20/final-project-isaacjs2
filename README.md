# Sudoku

[![license](https://img.shields.io/badge/license-MIT-green)](LICENSE)
[![docs](https://img.shields.io/badge/docs-yes-brightgreen)](docs/README.md)

**Author**: Isaac Schifferer - [`isaacjs2@illinois.edu`](mailto:example@illinois.edu)

## What is this project?
A simple app for playing Sudoku! It features puzzles of three difficulties and three different game modes.

Modes
- Standard: Solve a puzzle of the desired difficulty
- Time Trial: Solve three puzzles of the same difficulty
- Time Attack: Solve three puzzles, one of each difficulty


## Setting up the project
Dependencies and frameworks used
- [cmake](https://cmake.org/download/)
- [Cinder v0.9.3dev](https://libcinder.org/)
- [Visual Studio Community 2019](https://visualstudio.microsoft.com/downloads/)
- [CLion 2019.3.4](https://www.jetbrains.com/clion/download/)

To run the app, download any/all of the above items as needed, download the project as .zip file, and extract it into 
Cinder as a project

## Other libraries used
- [nlohmann/json](https://github.com/nlohmann/json)
- [sqlite](https://www.sqlitetutorial.net/)

## Controls
- Navigate the game board with your ***mouse*** or ***arrow keys***
- ***Right click*** to switch between pen and pencil mode
- Use ***backspace*** to clear the selected box
- When entering your name after you've solved a puzzle, hit ***enter*** to submit it