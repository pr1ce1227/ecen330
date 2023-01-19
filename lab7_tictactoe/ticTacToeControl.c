#include "ticTacToeControl.h"
#include "buttons.h"
#include "display.h"
#include "minimax.h"
#include "ticTacToe.h"
#include "ticTacToeDisplay.h"
#include "touchscreen.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Waiting Periods
#define INIT_INSTRUCTIONS_PERIOD 7
#define CPU_START_PERIOD 5

// Loop starting points
#define TOP 0
#define LFT 0

// Initialize variables
#define RESTART 0
#define BUTTON0_PRESSED 0x01

// Debug State machine Messages
#define INIT_MESSAGE "INIT STATE\n\r"
#define RESET_MESSAGE "RESET STATE \n\r"
#define PLAYER_SELECT_MESSAGE "PLAYER_SELECT STATE\n\r"
#define CPU_TURN_MESSAGE "CPU_TURN STATE\n\r"
#define HUMAN_TURN_MESSAGE "HUMAN_TURN STATE\n\r"
#define WAIT_MESSAGE "WAIT STATE\n\r"
#define FINSIHED_GAME_MESSAGE "FINISHED GAME STATE\n\r"

// Print Messages
#define MESSAGE_WIDTH DISPLAY_WIDTH / 5
#define MESSAGE_HEIGHT DISPLAY_HEIGHT / 3
#define START_MESSAGE                                                          \
  "      Touch board to play X \n\r\n\r                        -or-   "        \
  "\n\r\n\r    "                                                               \
  "    "                                                                       \
  "    "                                                                       \
  "Wait for the computer and "                                                 \
  "play O \n\r"

// State machine States
typedef enum {
  INIT,
  RESET,
  PLAYER_SELECT,
  CPU_TURN,
  HUMAN_TURN,
  WAIT,
  FINISHED_GAME
} ticTacToeControl_state_t;

// GLobal Variables
static uint16_t CPU_START_COUNT = RESTART;
static uint16_t INIT_SCREEN_COUNT = RESTART;
static tictactoe_board_t board;
static tictactoe_board_t *board_ptr = &board;
static ticTacToeControl_state_t currentState;
static bool erase = true;
static uint16_t count = RESTART;
static bool CPU_isX = true;
static bool valid_move = true;

// Test if Human selection was valid, takes in board, selection returns bool
static bool validTurn(tictactoe_board_t *board,
                      tictactoe_location_t possible_location) {
  bool valid;
  // Checks to see if selection already has an x or o return true or false
  if (board->squares[possible_location.row][possible_location.column] !=
      MINIMAX_EMPTY_SQUARE) {
    valid = false;
    valid_move = false;
  }

  else {
    valid = true;
    valid_move = true;
  }

  return valid;
}

// Prints currentState everytime it changes for debugging
static void debugStatePrintTICTACTOE() {
  static ticTacToeControl_state_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state
  // name over and over.
  if (previousState != currentState || firstPass) {
    firstPass = false; // previousState will be defined, firstPass is false.
    previousState =
        currentState;       // keep track of the last state that you were in.
    switch (currentState) { // This prints messages based upon the state that
                            // you were in.
    case INIT:
      printf(INIT_MESSAGE);
      break;
    case RESET:
      printf(RESET_MESSAGE);
      break;
    case PLAYER_SELECT:
      printf(PLAYER_SELECT_MESSAGE);
      break;
    case CPU_TURN:
      printf(CPU_TURN_MESSAGE);
      break;
    case HUMAN_TURN:
      printf(HUMAN_TURN_MESSAGE);
      break;
    case WAIT:
      printf(WAIT_MESSAGE);
      break;
    case FINISHED_GAME:
      printf(FINSIHED_GAME_MESSAGE);
      break;
    }
  }
}

// State machine running tick tac toe game
void ticTacToeControl_tick() {
  debugStatePrintTICTACTOE();
  static tictactoe_location_t nextMove;

  // State Transitions
  switch (currentState) {
  case (INIT):
    // Leave start message for 7 seconds, then erase
    if (count >= INIT_SCREEN_COUNT) {
      currentState = RESET;
      display_setCursor(MESSAGE_WIDTH, MESSAGE_HEIGHT);
      display_setTextColor(DISPLAY_DARK_BLUE);
      display_print(START_MESSAGE);
    }
    break;

  case (RESET):
    currentState = PLAYER_SELECT;
    count = RESTART;
    break;

  case (PLAYER_SELECT):
    // Wait 5 seconds before computer starts game, else human starts
    if (count >= CPU_START_COUNT) {
      currentState = CPU_TURN;
      count = RESTART;
      CPU_isX = true;
    }

    else if (touchscreen_get_status() == TOUCHSCREEN_PRESSED) {
      currentState = HUMAN_TURN;
      count = RESTART;
      CPU_isX = false;
    }

    else
      currentState = PLAYER_SELECT;
    break;

  case (CPU_TURN):
    // Check if the game is over, else let human take turn
    if (minimax_isGameOver(minimax_computeBoardScore(board_ptr, !CPU_isX)))
      currentState = FINISHED_GAME;
    else
      currentState = WAIT;
    break;

  case (HUMAN_TURN):
    // check if game is over, else check if move was valid then let CPU take
    // turn
    if (minimax_isGameOver(minimax_computeBoardScore(board_ptr, CPU_isX)))
      currentState = FINISHED_GAME;
    else if (valid_move)
      currentState = CPU_TURN;
    else {
      currentState = WAIT;
    }
    break;

  case (WAIT):
    // Check if game is finished, else wait for interrupt then let human go
    if (minimax_isGameOver(minimax_computeBoardScore(board_ptr, CPU_isX)))
      currentState = FINISHED_GAME;

    else if (touchscreen_get_status() == TOUCHSCREEN_PRESSED) {
      currentState = HUMAN_TURN;
    }

    else
      currentState = WAIT;
    break;

  case (FINISHED_GAME):
    // Wait for reset button to be pushed, the restart game
    if (buttons_read() == BUTTON0_PRESSED)
      currentState = RESET;
    else
      currentState = FINISHED_GAME;
    break;
  }

  // State Actions
  switch (currentState) {

  // Print start message
  case (INIT):
    count++;
    display_setCursor(MESSAGE_WIDTH, MESSAGE_HEIGHT);
    display_setTextColor(DISPLAY_WHITE);
    display_print(START_MESSAGE);
    break;

  case (RESET):
    minimax_initBoard(board_ptr);
    ticTacToeDisplay_init();
    // Clear the board, and erase the display
    for (uint8_t r = TOP; r < TICTACTOE_BOARD_ROWS; ++r) {
      for (uint8_t c = LFT; c < TICTACTOE_BOARD_COLUMNS; ++c) {
        tictactoe_location_t location;
        location.column = c;
        location.row = r;
        ticTacToeDisplay_drawX(location, erase);
        ticTacToeDisplay_drawO(location, erase);
      }
    }
    break;

    // counter for starting player
  case (PLAYER_SELECT):
    count++;
    break;

  case (CPU_TURN):
    nextMove = minimax_computeNextMove(board_ptr, CPU_isX);

    // Check if game is over, else check which player computer is, run minimax
    if (minimax_isGameOver(minimax_computeBoardScore(board_ptr, !CPU_isX))) {
      break;
    }

    else if (CPU_isX) {
      ticTacToeDisplay_drawX(nextMove, !erase);
      board_ptr->squares[nextMove.row][nextMove.column] = MINIMAX_X_SQUARE;
      touchscreen_ack_touch();
    }

    else {
      ticTacToeDisplay_drawO(nextMove, !erase);
      board_ptr->squares[nextMove.row][nextMove.column] = MINIMAX_O_SQUARE;
      touchscreen_ack_touch();
    }

    break;

  case (HUMAN_TURN):
    nextMove =
        ticTacToeDisplay_getLocationFromPoint(touchscreen_get_location());

    // check if game is over, else if move was valid,
    // else wether player is x or o, add move
    if (minimax_isGameOver(minimax_computeBoardScore(board_ptr, CPU_isX))) {
      break;
    }

    else if (validTurn(board_ptr, nextMove) == false) {
      touchscreen_ack_touch();
      break;
    }

    else if (CPU_isX) {
      ticTacToeDisplay_drawO(nextMove, !erase);
      board_ptr->squares[nextMove.row][nextMove.column] = MINIMAX_O_SQUARE;
      touchscreen_ack_touch();
    }

    else {
      ticTacToeDisplay_drawX(nextMove, !erase);
      board_ptr->squares[nextMove.row][nextMove.column] = MINIMAX_X_SQUARE;
      touchscreen_ack_touch();
    }
    break;

  case (WAIT):
    break;

  case (FINISHED_GAME):
    break;
  }
}

// Initialize the tic-tac-toe controller state machine,
// providing the tick period, in seconds.
// Initialize buttons and screen
void ticTacToeControl_init(double period_s) {
  display_init();
  CPU_START_COUNT = (double)CPU_START_PERIOD / period_s;
  INIT_SCREEN_COUNT = (double)INIT_INSTRUCTIONS_PERIOD / period_s;
  minimax_initBoard(board_ptr);
  display_fillScreen(DISPLAY_DARK_BLUE);
  buttons_init();
  currentState = INIT;
}