#ifndef SIMULATION_CPP
#define SIMULATION_CPP

#include "../h/simulation.h"

int move_evaluation(const Position &pos, const Move &m){
    PieceType attacker = pos.peek_piece_at(m.from()).type;
    PieceType target = pos.peek_piece_at(m.to()).type;

    return(target != NO_PIECE) ? yummy_table[attacker][target] : yummy_table[attacker][7];
}

Move strategy_weighted_random(const Position &pos, MoveList<> &moves){
    int scores[MaxChild + 1];
    scores[0] = 0;
    for(int i = 0; i < moves.size(); i++){
        scores[i] = move_evaluation(pos, moves[i]);
    }

    int prefix[MaxChild + 1];
    prefix[0] = scores[0];
    int total = scores[0];
    for(int i = 1; i < moves.size(); i++){
        prefix[i] = prefix[i - 1] + scores[i];
        total += scores[i];
    }

    // safeguard 1
    if(total <= 0) return moves[rng(moves.size())];

    int rnd = rng(total) + 1;
    int index = std::lower_bound(prefix, prefix + moves.size(), rnd) - prefix;
    // safeguard 2
    if(index >= moves.size()) index = moves.size() - 1;

    return moves[index];
}

int pos_simulation(Position &pos, int played_moves[total_type][SQUARE_NB][SQUARE_NB], const Color root_color, const int iter, int cur_depth){
    Position copy(pos);

    while (copy.winner() == NO_COLOR) {
        MoveList<> moves(copy);
        Move m = strategy_weighted_random(copy, moves);
        #ifdef RAVE
        cur_depth++;
        int type_index = (cur_depth & 1) ? (7 + copy.peek_piece_at(m.from()).type) : copy.peek_piece_at(m.from()).type;
        played_moves[type_index][m.from()][m.to()] = iter;
        #endif
        copy.do_move(m);
    }

    int diff = copy.count(root_color) - copy.count(Color(root_color ^ 1));

    if (copy.winner() == root_color) {
        return win_score + diff;
    } else if (copy.winner() == Mystery) {
        return diff;
    }
    return -win_score + diff;
}

#endif // SIMULATION_CPP