#include "minimax.h"
#include "ticTacToe.h"
#include <stdbool.h>
#include <stdio.h>

// Board size and location
#define BOARD_SIZE 9
#define RESET 0
#define TOP 0
#define MID 1
#define BOT 2
#define LFT 0
#define RGT 2

// Player Interface
#define PLAYER_X "X"
#define PLAYER_O "O"
#define EMPTY_SPACE " "
#define X_TURN_PROMPT "PLAYER X: \n\r"
#define O_TURN_PROMPT "PLAYER 0: \n\r"

// Initial max and min vals to decide best move
#define MAX_OUT_OF_BOUNDS 15
#define MIN_OUT_OF_BOUNDS -15

// Global Variables
tictactoe_location_t final_location;

// Prints current board to allow for deubugging in terminal
void printBoard(tictactoe_board_t *board) {
  for (uint8_t r = TOP; r < TICTACTOE_BOARD_ROWS; ++r) {
    for (uint8_t c = LFT; c < TICTACTOE_BOARD_COLUMNS; ++c) {
      if (board->squares[r][c] == MINIMAX_X_SQUARE) {
        printf(PLAYER_X);
      }

      else if (board->squares[r][c] == MINIMAX_O_SQUARE) {
        printf(PLAYER_O);
      }

      else {
        printf(EMPTY_SPACE);
      }
    }
    printf("\n\r");
  }
}

// Recursive function that finds the best move by playing out all possible moves
minimax_score_t minimax(tictactoe_board_t *board, bool is_Xs_turn,
                        uint16_t depth) {

  // Scores and moves array keep track of outcomes at each depth
  minimax_score_t scores[BOARD_SIZE];
  tictactoe_location_t moves[BOARD_SIZE];
  uint16_t count = RESET;

  // Evaluate board based on previous players turn
  if (minimax_isGameOver(minimax_computeBoardScore(board, !is_Xs_turn))) {
    return minimax_computeBoardScore(board, !is_Xs_turn);
  }

  // This loop will generate all possible boards and call
  // minimax recursively for every empty square
  for (uint8_t r = TOP; r < TICTACTOE_BOARD_ROWS; ++r) {
    for (uint8_t c = LFT; c < TICTACTOE_BOARD_COLUMNS; ++c) {
      if (board->squares[r][c] == MINIMAX_EMPTY_SQUARE) {

        // Simulate playing at this location
        board->squares[r][c] = is_Xs_turn ? MINIMAX_X_SQUARE : MINIMAX_O_SQUARE;

        // Recursively call minimax to get the best score, assuming player
        // choses to play at this location.
        minimax_score_t score = minimax(board, !is_Xs_turn, depth + 1);
        tictactoe_location_t location;
        location.row = r;
        location.column = c;
        scores[count] = score;
        moves[count] = location;
        ++count;

        // Undo the change to the board, step back one layer of depth
        board->squares[r][c] = MINIMAX_EMPTY_SQUARE;
      }
    }
  }

  // choose best moved based on whos turn it is and picking either
  // the max (x-turn) or min value (o-turn)
  if (is_Xs_turn) {
    minimax_score_t max = MIN_OUT_OF_BOUNDS;
    // Iterate over each option, counter holds total valid inputs
    // Select max value
    for (uint16_t i = RESET; i < count; ++i) {
      if (scores[i] > max) {
        max = scores[i];
        final_location = moves[i];
      }
    }

    return max;
  }

  else {
    minimax_score_t min = MAX_OUT_OF_BOUNDS;
    // Iterate over each option, counter holds total valid inputs
    // Select Min value
    for (uint16_t i = RESET; i < count; ++i) {
      if (scores[i] < min) {
        min = scores[i];
        final_location = moves[i];
      }
    }
    return min;
  }
}

// Main control for deciding next move, calls minimax to decide
// Prints whos turn it is to help understand terminal output
// Return final location of the best move
tictactoe_location_t minimax_computeNextMove(tictactoe_board_t *board,
                                             bool is_Xs_turn) {
  if (is_Xs_turn) {
    printf(X_TURN_PROMPT);
  }

  else {
    printf(O_TURN_PROMPT);
  }

  printBoard(board);
  minimax(board, is_Xs_turn, RESET);
  return final_location;
}

// Returns the score of the board.
// This returns one of 4 values: MINIMAX_X_WINNING_SCORE,
// MINIMAX_O_WINNING_SCORE, MINIMAX_DRAW_SCORE, MINIMAX_NOT_ENDGAME
// Note: the is_Xs_turn argument indicates which player just took their
// turn and makes it possible to speed up this function.
minimax_score_t minimax_computeBoardScore(tictactoe_board_t *board,
                                          bool is_Xs_turn) {
  // Check for x wins... else o wins
  if (is_Xs_turn) {
    // Check for X win in rows
    for (uint8_t r = TOP; r < TICTACTOE_BOARD_ROWS; ++r) {
      if ((board->squares[r][LFT] == MINIMAX_X_SQUARE) &&
          (board->squares[r][MID] == MINIMAX_X_SQUARE) &&
          (board->squares[r][RGT] == MINIMAX_X_SQUARE)) {
        // printf("x wins\n\r");
        return MINIMAX_X_WINNING_SCORE;
      }
    }

    // Check for X win in columns
    for (uint8_t c = LFT; c < TICTACTOE_BOARD_COLUMNS; ++c) {
      if ((board->squares[TOP][c] == MINIMAX_X_SQUARE) &&
          (board->squares[MID][c] == MINIMAX_X_SQUARE) &&
          (board->squares[BOT][c] == MINIMAX_X_SQUARE)) {
        // printf("x wins\n\r");
        return MINIMAX_X_WINNING_SCORE;
      }
    }

    // check for x wins in \ diagonal
    if ((board->squares[TOP][LFT] == MINIMAX_X_SQUARE) &&
        (board->squares[MID][MID] == MINIMAX_X_SQUARE) &&
        (board->squares[BOT][RGT] == MINIMAX_X_SQUARE)) {
      // printf("x wins\n\r");
      return MINIMAX_X_WINNING_SCORE;
    }

    // check for x win in / diagonal
    if ((board->squares[TOP][RGT] == MINIMAX_X_SQUARE) &&
        (board->squares[MID][MID] == MINIMAX_X_SQUARE) &&
        (board->squares[BOT][LFT] == MINIMAX_X_SQUARE)) {
      // printf("x wins\n\r");
      return MINIMAX_X_WINNING_SCORE;
    }

    // check to see if board has an empty space
    for (uint8_t r = TOP; r < TICTACTOE_BOARD_ROWS; ++r) {
      for (uint8_t c = LFT; c < TICTACTOE_BOARD_COLUMNS; ++c) {
        if (board->squares[r][c] == MINIMAX_EMPTY_SQUARE) {
          return MINIMAX_NOT_ENDGAME;
        }
      }
    }

  }

  else {
    // Check for O win in rows
    for (uint8_t r = TOP; r < TICTACTOE_BOARD_ROWS; ++r) {
      if ((board->squares[r][LFT] == MINIMAX_O_SQUARE) &&
          (board->squares[r][MID] == MINIMAX_O_SQUARE) &&
          (board->squares[r][RGT] == MINIMAX_O_SQUARE)) {
        // printf("O wins\n\r");
        return MINIMAX_O_WINNING_SCORE;
      }
    }

    // Check for O win in columns
    for (uint8_t c = LFT; c < TICTACTOE_BOARD_COLUMNS; ++c) {
      if ((board->squares[TOP][c] == MINIMAX_O_SQUARE) &&
          (board->squares[MID][c] == MINIMAX_O_SQUARE) &&
          (board->squares[BOT][c] == MINIMAX_O_SQUARE)) {
        // printf("O wins\n\r");
        return MINIMAX_O_WINNING_SCORE;
      }
    }

    // check for O wins in \ diagonal
    if ((board->squares[TOP][LFT] == MINIMAX_O_SQUARE) &&
        (board->squares[MID][MID] == MINIMAX_O_SQUARE) &&
        (board->squares[BOT][RGT] == MINIMAX_O_SQUARE)) {
      // printf("O wins\n\r");
      return MINIMAX_O_WINNING_SCORE;
    }

    // check for O win in / diagonal
    if ((board->squares[TOP][RGT] == MINIMAX_O_SQUARE) &&
        (board->squares[MID][MID] == MINIMAX_O_SQUARE) &&
        (board->squares[BOT][LFT] == MINIMAX_O_SQUARE)) {
      // printf("O wins\n\r");
      return MINIMAX_O_WINNING_SCORE;
    }

    // check to see if board has an empty space
    for (uint8_t r = TOP; r < TICTACTOE_BOARD_ROWS; ++r) {
      for (uint8_t c = LFT; c < TICTACTOE_BOARD_COLUMNS; ++c) {
        if (board->squares[r][c] == MINIMAX_EMPTY_SQUARE) {
          return MINIMAX_NOT_ENDGAME;
        }
      }
    }
  }

  // Default, if no winners and board is full than a draw
  return MINIMAX_DRAW_SCORE;
}

// Init the board to all empty squares.
// Set initial move to top left square
void minimax_initBoard(tictactoe_board_t *board) {
  final_location.row = RESET;
  final_location.column = RESET;
  for (uint8_t r = TOP; r < TICTACTOE_BOARD_ROWS; ++r) {
    for (uint8_t c = LFT; c < TICTACTOE_BOARD_COLUMNS; ++c) {
      board->squares[r][c] = MINIMAX_EMPTY_SQUARE;
    }
  }
}

// Determine that the game is over by looking at the score.
bool minimax_isGameOver(minimax_score_t score) {
  if (score == MINIMAX_NOT_ENDGAME) {
    return false;
  }

  else {
    return true;
  }
}