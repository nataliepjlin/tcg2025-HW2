#ifndef SIMULATION_CPP
#define SIMULATION_CPP

#include "../h/simulation.h"

bool is_risky_move(const Position &pos, const Move &m){
    Direction dirs[] = {NORTH, SOUTH, EAST, WEST};
    Square to = m.to();
    Piece my_piece = pos.peek_piece_at(m.from());
    for(Direction dir: dirs){
        Square adj = Square(to + dir);
        if(is_okay(adj)){
            Piece adj_piece = pos.peek_piece_at(adj);
            if(adj_piece.side != my_piece.side && adj_piece.side != NO_COLOR
            && adj_piece.type != Cannon && adj_piece.type > my_piece.type){
                return true;
            }
        }
    }
    return false;
}

int move_evaluation(const Position &pos, const Move &m){
    PieceType attacker = pos.peek_piece_at(m.from()).type;
    PieceType target = pos.peek_piece_at(m.to()).type;

    return(target != NO_PIECE) ? yummy_table[attacker][target] : is_risky_move(pos, m) ? 5 : yummy_table[attacker][7];
}

Move strategy_weighted_random(const Position &pos, MoveList<> &moves){
    if(moves.size() == 0) {
        // Safety check - should not happen
        return Move();
    }
    
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

int pos_simulation(Position &pos, long long played_moves[total_type][SQUARE_NB][SQUARE_NB], const Color root_color, const long long iter, long long cur_depth){
    Position copy(pos);
    
    const int MAX_SIM_MOVES = 200; // Prevent infinite simulation
    int move_count = 0;

    while (copy.winner() == NO_COLOR && move_count < MAX_SIM_MOVES) {
        MoveList<> moves(copy);
        if(moves.size() == 0) break; // No moves available
        
        Move m = strategy_weighted_random(copy, moves);
        cur_depth++;
        int type_index = (cur_depth & 1) ? (7 + copy.peek_piece_at(m.from()).type) : copy.peek_piece_at(m.from()).type;
        played_moves[type_index][m.from()][m.to()] = iter;
        copy.do_move(m);
        move_count++;
    }

    int diff = copy.count(root_color) - copy.count(Color(root_color ^ 1));

    if (copy.winner() == root_color) {
        return win_score + diff;
    } else if (copy.winner() == Mystery) {
        return diff;
    } else if (copy.winner() == NO_COLOR) {
        // Simulation timed out, return current score
        return diff;
    }
    return -win_score + diff;
}

#endif // SIMULATION_CPP