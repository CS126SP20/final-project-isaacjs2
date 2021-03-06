# Development

---
**4/20/20** 
- Started setting up project with CPR
- Still having trouble with CMake

**4/21/20** 
- Got CMake file to build
- Started adding helpers for draw function
- Added basic leaderboard database

**4/24/20**
- Added menu screen

**4/26/20**
- Made menu setup abstract and scalable
- Added basic grid for game
- Added a bunch of instance variables to track information about the state of the game/app
- Added functionality for clicking on things
- Added logic for filling in numbers and started doing the same for pencil marks

**4/27/20**
- Added indicator in game screen to show if the user is penciling or not
- Added button to go back to the menu
- Updated draw() to use more instance variables and helpers
- Cleaned up long lines

**4/28/20**
- Added printing for pencil marks
- Added functionality for deleting numbers
- Added sample game boards
- Set up engine class and started moving game logic to it

**4/29/20**
- Fixed bugs relating to moving code from app to engine
- Finished moving game-related variables and functions to engine class
- Made given numbers unmodifiable on the game board
- Added difficulty selector in menu

**4/30/20**
- Made buttons to toggle difficulty and instructions
- Wrote game instructions for game screen

**5/1/20**
- Added a check board button that locks in correct entries and highlights wrong ones
- Started working on Game Over/Win screen

**5/3/20**
- Added functionality for the player entering their name at the end of the game
- Started implementing leaderboard database

**5/4/20**
- Added leaderboard to game over screen
- Added the ability to play again without closing the app
- Added a timer for determining the leaderboard
- Added instructions to the menu
- Added board randomization when starting a game
- Added the other two game modes
- The times from all 3 modes are going to the same leaderboard, so I need to make a 3rd column in my database table and 
adjust my database functions

**5/5/20**
- Added arrow key navigation on the game board
- Fixed a bunch of bugs having to do with playing multiple games
- Added a hint button
- Cleaned up code with more helper functions, comments, and more logical function order
- Changed leaderboard to sort by game mode and difficulty

**5/6/20**
- Wrote tests
- Fixed up proposal
- Wrote README