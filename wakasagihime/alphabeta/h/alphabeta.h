#ifndef ALPHABETA_H
#define ALPHABETA_H
#include "../../lib/chess.h"
#include "../../lib/movegen.h"
#include "../../lib/helper.h"
#include "../../mcts/h/simulation.h" // for move_evaluation
#include <algorithm>
#include <fstream>
#include <chrono>

bool is_terminal(Position &pos);
int F3(Position &pos, int alpha, int beta, int depth);
Move alphabeta_search(Position &pos);
bool move_compare(const Position &pos, const Move &a, const Move &b);
int pos_score(Position &pos, const Color cur_color);

const int Piece_Value[] = {
    7,  // General = 0
    8,  // Advisor = 1
    6,  // Elephant = 2
    5,  // Chariot = 3
    3,  // Horse = 4
    9,  // Cannon = 5
    1   // Soldier = 6
};
const int ab_win_score = 10000;

#endif // ALPHABETA_H