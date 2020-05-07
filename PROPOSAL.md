# Sudoku

Author: Isaac Schifferer

---

I want to recreate Sudoku using Cinder, as well as nlohmann/json and sqlite.
I plan to utilize Cinder to generate a game board where the user can enter numbers, pencil marks, and check their answers.
Additionally, I will have menu that displays the different game modes as well as a couple of settings like a difficulty
selector. Lastly, there will be a game over screen displaying the user's time and a leaderboard with the top times of
that game mode/difficulty combo. I will use sqlite for storing leaderboard data, and nlohmann/json for interpreting board 
files and putting the information on the game board.

The only relevant experience I have going into this project is knowing how APIs work in general as well as how to play 
Sudoku. I expect the hardest part of the project to be setting up the board and detecting when each box is clicked.

In the first week, I want to get the libraries I will use set up as well as make skeletons of the different screens of 
the game. In the second week, I want to work on the gameplay, the ability to select and edit entries on the board, as 
well as add clicking functionality for the app. In the last week, I will set up the leaderboard and connect game board 
files to the app to make working games.

Ideas for extensions if I finish the base project early enough:
- A hint system
- An error checking system
- Ability to mover around the game board with arrow keys
- Tutorial-like instructions throughout the app

sqlite: https://www.sqlitetutorial.net/

nlohmann/json: https://github.com/nlohmann/json