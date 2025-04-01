#ifndef TTT_H   //headers inclusion protection
#define TTT_H

#include <getopt.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <limits.h> //get the maximal and minimal values of a type


typedef unsigned int uint; // defines an alias for unsigned int called uint

// Constant character to represent the cross player, the round, none or both of
// them (this last one is usefull for tie game)
#define PLAYER_O 'O'
#define PLAYER_X 'X'
#define NOBODY '.'
#define BOTH '#'

typedef char player_t; //defines a "new" type called player which is a char

// Constant defining the 9 possible positions in a tic tac toe, any of this
// position and none of this position

#define TOPLEFT 0
#define TOPCENTER 1
#define TOPRIGHT 2
#define MIDLEFT 3
#define MIDCENTER 4
#define MIDRIGHT 5
#define BOTTOMLEFT 6
#define BOTTOMCENTER 7
#define BOTTOMRIGHT 8
#define NONE 9

typedef int location_t; //defines a "new" type called location which is a int

#define TICTACTOE_SIZE 9
#define TICTACTOE_WIDTH 3


// A tic tac toe game structure
typedef struct {
  player_t content[TICTACTOE_SIZE];  // array of TICTACTOE_SIZE player_t defining content of
                                     // each the TICTACTOE_SIZE cells of a tic tac toe defined
                                     // from top left to bottom right
  player_t winner;                   // player who have won the tic tac toe game; BOTH in case of
                                     // a tie game
  bool is_cross_human;
  bool is_round_human;                                   
  player_t current_player;           // The current player
  location_t history[TICTACTOE_SIZE];// full history of the played moves - there are at most TICTACTOE_SIZE of them
                                     // much more simple and efficient thatn a stack
} ttt_game;

/*
- Example -
content = [PLAYER_X, NOBODY, PLAYER_O, NOBODY, PLAYER_X, PLAYER_O, NOBODY, NOBODY, PLAYER_X]
X . O
. X O
. . X
winner = PLAYER_X
history = [TOPLEFT, MIDRIGHT, MIDCENTER, TOPRIGHT, BOTTOMRIGHT, NONE, NONE, NONE, NONE]
X played topleft, O played midright, X played midcenter, O played topright, X played bottomright and won
*/


/*!
 * This function allocates an ttt_game structure corresponding to a new
 * game ready to be played.
 *
 * \param is_round_human a boolean stating if round player is human or automatic.
 * \param is_cross_human a boolean stating if cross player is human or automatic.
 * \return a reference to the ttt_game memory space allocated, NULL in case
 * of allocation problem.
 */
ttt_game *create_game(bool is_round_human, bool is_cross_human);

/*!
 * This function free all the memory used by a given ttt_game structure which
 * reference is given.
 *
 * \param g a pointer on a ttt_game to be freed.
 */
void free_game(ttt_game *g);

/*!
 * This function set the winner of the corresponding ttt_game.
 *
 * \param g a non NULL pointer on a ttt_game whose winner field will be updated.
 */
void set_game_winner(ttt_game *g);


// Minimax algorithm is inspired from https://www.youtube.com/watch?v=trKjYdBASyQ
/*!
 * This function computes the score of the given player.
 *
 * \param g a non NULL pointer  on a ttt_game.
 * \param player the player for which we try to compute the score. 
 * \param is_maximizing a boolean stating if we should maximize or minize the score of the given player.
 * \return a score.
 */
int minimax(ttt_game *g, player_t player, bool is_maximizing);

/*!
 * This function plays the best move possible for the current_player of a ttt_game using minimax algorithm.
 *
 * \param g a pointer on a ttt_game.
 */
void play_best_move(ttt_game *g);

/*!
 * This function plays the next move either as a human or automatic depending on the current_player.
 *
 * \param win a pointer on a ttt_game.
 * \param g a pointer on a ttt_game.
 */
void play_next_move(WINDOW *win, ttt_game *g);


/*!
 * This function initializes the display using ncurses.
 *
 * \return a reference to the WINDOW corresponding to the game display area.
 */
WINDOW *create_UI();


/*!
 * This function displays the current scores for both players.
 *
 * \param score_cross score for X player.
 * \param score_round score for O player.
 */
void draw_score(uint score_cross, uint score_round);

/*!
 * This function displays the current state of the game.
 *
 * \param win a pointer on a ttt_game.
 * \param g a pointer on a ttt_game.
 * \param current current cursir location
 */
void draw_game(WINDOW *win, ttt_game *g, location_t current);


/*!
 * This function displays the current winner of the game.
 *
 * \param win a pointer on a ttt_game.
 * \param g a pointer on a ttt_game.
 */
void display_winner(WINDOW *win, ttt_game *g);

#endif /*TTT*/
