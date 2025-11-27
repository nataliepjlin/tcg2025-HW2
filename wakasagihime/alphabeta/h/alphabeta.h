#ifndef ALPHABETA_H
#define ALPHABETA_H
#include "../../lib/chess.h"
#include "../../lib/movegen.h"
#include "../../lib/helper.h"
#include "../../mcts/h/simulation.h"

bool is_terminal(const Position &pos);
int F3(const Position &pos, int alpha, int beta);
Move alphabeta_search(const Position &pos);
#endif // ALPHABETA_H