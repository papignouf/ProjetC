#include "tictactoe.h"

ttt_game *create_game(bool is_round_human, bool is_cross_human)
{
    ttt_game *new = malloc(sizeof(ttt_game));
    if (new == NULL)
    {
        printf("Allocation problem in create_game function"); 
        exit(EXIT_FAILURE);
    }
    new->winner = NOBODY;
    for (uint i = 0; i < TICTACTOE_SIZE; i++)
    {
        new->content[i] = NOBODY;
        new->history[i] = NONE;
    }
    new->current_player = PLAYER_X;
    new->is_cross_human = is_cross_human;
    new->is_round_human = is_round_human;
    return new;
}

void free_game(ttt_game *g)
{
    free(g); // if g is NULL, nothing is done
}

void set_game_winner(ttt_game *g)
{
    /* LINES */
    // UPPERLINE
    if (g->content[TOPLEFT] != NOBODY && g->content[TOPLEFT] == g->content[TOPCENTER] &&
        g->content[TOPCENTER] == g->content[TOPRIGHT])
    {
        g->winner = g->content[TOPLEFT];
        return;
    }
    // MIDLINE
    if (g->content[MIDLEFT] != NOBODY && g->content[MIDLEFT] == g->content[MIDCENTER] &&
        g->content[MIDCENTER] == g->content[MIDRIGHT])
    {
        g->winner = g->content[MIDLEFT];
        return;
    }
    // BOTTOMLINE
    if (g->content[BOTTOMLEFT] != NOBODY && g->content[BOTTOMLEFT] == g->content[BOTTOMCENTER] &&
        g->content[BOTTOMCENTER] == g->content[BOTTOMRIGHT])
    {
        g->winner = g->content[BOTTOMLEFT];
        return;
    }
    /* COLUMN */
    // LEFTCOLUMN
    if (g->content[TOPLEFT] != NOBODY && g->content[TOPLEFT] == g->content[MIDLEFT] &&
        g->content[TOPLEFT] == g->content[BOTTOMLEFT])
    {
        g->winner = g->content[TOPLEFT];
        return;
    }
    // MIDCOLUMN
    if (g->content[TOPCENTER] != NOBODY && g->content[TOPCENTER] == g->content[MIDCENTER] &&
        g->content[TOPCENTER] == g->content[BOTTOMCENTER])
    {
        g->winner = g->content[TOPCENTER];
        return;
    }
    // RIGHTCOLUMN
    if (g->content[TOPRIGHT] != NOBODY && g->content[TOPRIGHT] == g->content[MIDRIGHT] &&
        g->content[TOPRIGHT] == g->content[BOTTOMRIGHT])
    {
        g->winner = g->content[TOPRIGHT];
        return;
    }
    /* Diagonals */
    // LEFTTORIGHT DIAGONAL
    if (g->content[TOPLEFT] != NOBODY && g->content[TOPLEFT] == g->content[MIDCENTER] &&
        g->content[TOPLEFT] == g->content[BOTTOMRIGHT])
    {
        g->winner = g->content[TOPLEFT];
        return;
    }
    // RIGHTTOLEFT DIAGONAL
    if (g->content[TOPRIGHT] != NOBODY && g->content[TOPRIGHT] == g->content[MIDCENTER] &&
        g->content[TOPRIGHT] == g->content[BOTTOMLEFT])
    {
        g->winner = g->content[TOPRIGHT];
        return;
    }
    /* TIE ? */
    for (uint i = 0; i < TICTACTOE_SIZE; i++)
    {
        if (g->content[i] == NOBODY)
        {
            g->winner = NOBODY;
            return;
        }
    }
    g->winner = BOTH;
}

void draw_score(uint score_cross, uint score_round)
{
    mvwprintw(stdscr, 2, 2, "Score X:%u O:%u", score_cross, score_round);
}

void draw_game(WINDOW *win, ttt_game *g, location_t current_location)
{
    //display an empty game
    mvwprintw(win, 2, 3, " | | ");
    mvwprintw(win, 3, 3, "-----");
    mvwprintw(win, 4, 3, " | | ");
    mvwprintw(win, 5, 3, "-----");
    mvwprintw(win, 6, 3, " | | ");

    //rewrite the game content based on g->content
    for (location_t cell = TOPLEFT; cell <= BOTTOMRIGHT; cell++)
    {
        if (current_location == cell)
        {
            wattron(win, COLOR_PAIR(1));//highlight the current location 
        }
        mvwaddch(win, 2 + 2 * (cell / 3), 3 + 2 * (cell % 3), g->content[cell]);//add a single character from PLAYER_X, PLAYER_O or NOBODY
        // 2, 3 is the current top left position 
        if (current_location == cell)
        {
            wattroff(win, COLOR_PAIR(1));//remove the highlight for the other location
        }
    }
    wrefresh(win);//force the update of the win window
}

WINDOW *create_UI()
{
    //initialization of ncurses
    initscr();
    raw();
    noecho();
    start_color();
    keypad(stdscr, TRUE);
    init_pair(1, COLOR_BLACK, COLOR_RED); // color pair with font in black and background in red

    curs_set(0); //remove the default cursor

    WINDOW *full_UI = subwin(stdscr, 15, 30, 0, 0); // A subwindow in position 0,0 of width 30 characters and height 15 characters
    box(full_UI, ACS_VLINE, ACS_HLINE); // make its border visible
    mvwprintw(stdscr, 0, 2, "(q)uit (n)ew game"); // At position line 0, column 2, display the message
    
    WINDOW *score = subwin(stdscr, 3, 28, 1, 1); // A subwindow in position 1,1 of width 28 characters and height 3 characters
    box(score, ACS_VLINE, ACS_HLINE);// make its border visible
    draw_score(0, 0);

    WINDOW *game_window = subwin(stdscr, 9, 11, 4, 9); // A subwindow in position 9,4 of width 9 characters and height 11 characters
    box(game_window, ACS_VLINE, ACS_HLINE);// make its border visible
    return game_window;
}

void display_winner(WINDOW *win, ttt_game *g)
{    
    draw_game(win, g, TOPLEFT);
    if(g->winner== PLAYER_O){
        mvwprintw(stdscr, 13, 2, "The winner is player O");
    }
    if(g->winner== PLAYER_X){
        mvwprintw(stdscr, 13, 2, "The winner is player X");
    }
    if(g->winner== BOTH){
        mvwprintw(stdscr, 13, 2, "The winner is nobody. ");
    }
    wrefresh(stdscr);
}

int minimax(ttt_game *g, player_t player, bool is_maximizing)
{
    if (g->winner == player)
    {
        return 1; //I WIN
    }
    if (g->winner == BOTH)
    {
        return 0; //TIE
    }
    if (g->winner != NOBODY)
    {
        return -1; // I LOSE
    }
    int best_score;
    if (is_maximizing)
    {
        best_score = INT_MIN;
    }
    else
    {
        best_score = INT_MAX;
    }
    for (int i = TOPLEFT; i <= BOTTOMRIGHT; i++)
    {
        if (g->content[i] == NOBODY)
        {
            //play the position
            g->content[i] = g->current_player;
            if (g->current_player == PLAYER_O)
            {
                g->current_player = PLAYER_X;
            }
            else
            {
                g->current_player = PLAYER_O;
            }
            set_game_winner(g);
            int score = minimax(g, player, !is_maximizing);
            // unplay the position
            g->content[i] = NOBODY;
            if (g->current_player == PLAYER_O)
            {
                g->current_player = PLAYER_X;
            }
            else
            {
                g->current_player = PLAYER_O;
            }
            g->winner = NOBODY;
            if (is_maximizing)
            {
                if(best_score > score) {
                    best_score = best_score;
                }else{
                    best_score = score;
                }
            }
            else
            {
                if(best_score < score){
                    best_score = best_score;
                }else{
                    best_score = score;
                }
            }
        }
    }
    return best_score;
}

void play_best_move(ttt_game *g)
{
    int best_score = INT_MIN;
    location_t best_position = NONE;
    for (location_t current = TOPLEFT; current <= BOTTOMRIGHT; current++)
    {
        //tries every possible location
        if (g->content[current] == NOBODY)
        { // current position is free
            // play the position
            g->content[current] = g->current_player;
            //change the current player
            if (g->current_player == PLAYER_O)
            {
                g->current_player = PLAYER_X;
            }
            else
            {
                g->current_player = PLAYER_O;
            }
            // compute a possible winner
            set_game_winner(g);
            //computes a score for the corresponding position
            int score = minimax(g, g->current_player, false);
            // unplay the position
            g->content[current] = NOBODY;
            if (g->current_player == PLAYER_O)
            {
                g->current_player = PLAYER_X;
            }
            else
            {
                g->current_player = PLAYER_O;
            }
            g->winner = NOBODY;
            if (score > best_score)
            {
                //memorize the best location among all
                best_score = score;
                best_position = current;
            }
        }
    }

    g->content[best_position] = g->current_player;
    if (g->current_player == PLAYER_O)
    {
        g->current_player = PLAYER_X;
    }
    else
    {
        g->current_player = PLAYER_O;
    }
    uint i = 0;
    while (g->history[i] != NONE)
    {
        i = i + 1;
    }
    g->history[i] = best_position;
    set_game_winner(g);
}

void play_next_move(WINDOW *win, ttt_game *g)
{
    bool current_player_is_human = true;
    if ((g->current_player == PLAYER_O) && (g->is_round_human == false))
    {
        current_player_is_human = false;
    }
    if ((g->current_player == PLAYER_X) && (g->is_cross_human == false))
    {
        current_player_is_human = false;
    }
    location_t current_location = TOPLEFT;
    if (current_player_is_human == false)
    { // ask minimax algorithm
        play_best_move(g);
        draw_game(win, g, current_location);
    }
    else
    { // ask ncurse
        draw_game(win, g, current_location);
        mvwprintw(stdscr, 13, 2, "Player to play:%c", g->current_player);
        while (true)
        {
            int ch = wgetch(stdscr);
            switch (ch)
            {
            case '\n'://Key 'enter'
                if (g->content[current_location] == NOBODY)
                { // current position is free
                    //it is modified to the current player
                    g->content[current_location] = g->current_player;
                    //change the current player
                    if (g->current_player == PLAYER_O)
                    {
                        g->current_player = PLAYER_X;
                    }
                    else
                    {
                        g->current_player = PLAYER_O;
                    }
                    //update the history
                    //find the first empty cell of the history 
                    uint i = 0;
                    while (g->history[i] != NONE)
                    {
                        i = i + 1;
                    }
                    //put the current location in it
                    g->history[i] = current_location;
                    //compute the possible winner
                    set_game_winner(g);
                    //dras the current state of the game
                    draw_game(win, g, current_location);
                    return;//quit the function
                }
                break;
            case KEY_RIGHT:
                //available for any column expect the last one
                if (current_location % TICTACTOE_WIDTH < TICTACTOE_WIDTH - 1)
                {
                    current_location++;
                }
                break;
            case KEY_LEFT:
                //available for any column expect the first one
                if (current_location % TICTACTOE_WIDTH > 0)
                {
                    current_location--;
                }
                break;
            case KEY_DOWN:
                //available for any line expect the last one
                if (current_location < BOTTOMLEFT)
                {
                    current_location = current_location + TICTACTOE_WIDTH;
                }
                break;
            case KEY_UP:
                //available for any line expect the first one
                if (current_location > TOPRIGHT)
                {
                    current_location = current_location - TICTACTOE_WIDTH;
                }
                break;
            }
            draw_game(win, g, current_location);
        }
    }
}

int main(int argc, char *argv[])
{
    int optc; // variable which will contain the current short option from the command line

    // Defines the tactic for each player : either human (keyboard - by default) or automatic
    bool is_cross_human = true;
    bool is_round_human = true;
   

    /* ***** Parsing options ***** */
    while ((optc = getopt(argc, argv, "xoh")) != -1)
    { // we only consider the options x, o and h
        switch (optc)
        {
        case 'x': /* Setting cross player tactic */
            is_cross_human = false;
            break;
        case 'o': /* Setting round player tactic */
            is_round_human = false;
            break;
        case 'h': /* Display usage and exit */
            printf(
                "Usage: %s ([-x|-o|-h])\n"
                "Play a tictactoe game with human or program players.\n"
                " -x, set 'X' player as an IA\n"
                " -o, set 'O' player as an IA\n"
                " -h, --help display this help and exit\n"
                "\n",
                argv[0]);                                         // we adapt the message to the current name of the program
            exit(EXIT_SUCCESS);                                   // stop the program with a SUCCESS status
        default:                                                  /* unvalid option */
            printf("Try '%s -h' for more information.", argv[0]); // we adapt the message to the current name of the program
            exit(EXIT_FAILURE);                                   // stop the program with a FAILURE status
        }
    }

    // game loop
    WINDOW *game_window = create_UI();
    uint score_cross = 0;
    uint score_round = 0;
    int ch = 'n';
    ttt_game *g = NULL; // current game
    ttt_game *replay = NULL; // a copy of the current game to be replayed

    do
    {
        ch = wgetch(stdscr);// retrieve a caracter from the UI - STanDard SCReen
        switch (ch)
        {
        case 'n':
            if (g != NULL) //in case of a current game
            {
                free_game(g); //free the corresponding memory
            }
            g = create_game(is_round_human, is_cross_human);
            draw_game(game_window, g, TOPLEFT);
            mvwprintw(stdscr, 13, 2, "Player to play:X");//Display the message at column 2 and row 13 of the standard screen
            while (g->winner == NOBODY)
            {
                play_next_move(game_window, g);
            }
            display_winner(game_window, g);
            //score handling
            if (g->winner == PLAYER_X)
            {
                score_cross++;
            }
            if (g->winner == PLAYER_O)
            {
                score_round++;
            }
            draw_score(score_cross, score_round);
            mvwprintw(stdscr, 0, 2, "(q)uit (n)ew game (r)eplay");
            break;
        case 'r':
            if (g==NULL){
                //in case of no current game, just ignore the request
                break;
            }
            replay = create_game(is_round_human, is_cross_human);
            draw_game(game_window, replay, TOPLEFT);
            uint i = 0;
            while ((g->history[i] != NONE) && (i < TICTACTOE_SIZE))
            {
                replay->content[g->history[i]] = replay->current_player;
                if (replay->current_player == PLAYER_O)
                {
                    replay->current_player = PLAYER_X;
                }
                else
                {
                    replay->current_player = PLAYER_O;
                }
                draw_game(game_window, replay, g->history[i]);
                sleep(1);//wait for a second
                refresh();
                i = i + 1;
            }
            free_game(replay);
            break;
        }

    } while (ch != 'q');
    endwin(); // free all ncurses related memory
    return EXIT_SUCCESS; // The game went well until the end
}
