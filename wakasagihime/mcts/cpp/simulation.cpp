#ifndef SIMULATION_CPP
#define SIMULATION_CPP

#include "../h/simulation.h"

int move_evaluation(const Position &pos, const Move &m){
    PieceType attacker = pos.peek_piece_at(m.from()).type;
    PieceType target = pos.peek_piece_at(m.to()).type;

    return (target != NO_PIECE) ? yummy_table[attacker][target] : other_move_score;
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

int pos_simulation(Position &pos, std::vector<AmafMove> &played_moves){
    Position copy(pos);

    int depth_count = 0;
    while (copy.winner() == NO_COLOR) {
        MoveList<> moves(copy);
        Move m = strategy_weighted_random(copy, moves);
        #ifdef RAVE
        if(depth_count < AMAF_CUTOFF){
            played_moves.push_back({m, copy.peek_piece_at(m.from()).type, copy.peek_piece_at(m.from()).side});
        }
        depth_count++;
        #endif
        copy.do_move(m);
    }
    if (copy.winner() == pos.due_up()) {
        return 1;
    } else if (copy.winner() == Mystery) {
        return 0;
    }
    return -1;
}

#endif // SIMULATION_CPP