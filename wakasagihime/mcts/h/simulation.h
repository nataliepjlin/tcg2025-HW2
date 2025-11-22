#ifndef SIMULATION_H
#define SIMULATION_H

#include "../../lib/types.h"
#include "../../lib/movegen.h"
#include "../../lib/chess.h"
#include "../../lib/helper.h"
#include <vector>


const int AMAF_CUTOFF = 15; // Only record the first 15 moves of random play

int pos_simulation(Position &pos, Move (*strategy)(MoveList<> &moves), std::vector<Move> &played_moves);

#endif // SIMULATION_H