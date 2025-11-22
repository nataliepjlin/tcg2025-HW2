#ifndef SIMULATION_CPP
#define SIMULATION_CPP

#include "../h/simulation.h"

int pos_simulation(Position &pos, Move (*strategy)(MoveList<> &moves), std::vector<Move> &played_moves){
    Position copy(pos);

    int depth_count = 0;
    while (copy.winner() == NO_COLOR) {
        MoveList<> moves(copy);
        Move m = strategy(moves);
        copy.do_move(m);
        
        if(depth_count < AMAF_CUTOFF){
            played_moves.push_back(m);
        }
        depth_count++;
    }
    if (copy.winner() == pos.due_up()) {
        return 1;
    } else if (copy.winner() == Mystery) {
        return 0;
    }
    return -1;
}

#endif // SIMULATION_CPP