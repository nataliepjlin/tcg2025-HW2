#ifndef ALPHABETA_CPP
#define ALPHABETA_CPP

#include "../h/alphabeta.h"

bool is_terminal(const Position &pos){
    return pos.winner() != NO_COLOR;
}

int F3(const Position &pos, int alpha, int beta){
    if(is_terminal(pos)){
        int diff = pos.count(pos.due_up()) - pos.count(Color(pos.due_up() ^ 1));
        if(pos.winner() == pos.due_up())
            return win_score + diff;
        else if(pos.winner() == Mystery)
            return diff;
        else
            return -win_score + diff;
    }

    MoveList<> moves(pos);
    int mx = -1e9;
    for(int i = 0; i < moves.size(); i++){
        Position copy(pos);
        copy.do_move(moves[i]);
        int t = -F3(copy, -beta, -std::max(alpha, mx));
        if(t > mx)
            mx = t;
        if(mx >= beta)
            return mx;
    }
    return mx;
}

Move alphabeta_search(const Position &pos){
    MoveList<> moves(pos);
    Move best_move = moves[0];
    int mx = -1e9;
    for(int i = 0; i < moves.size(); i++){
        Position copy(pos);
        copy.do_move(moves[i]);
        int t = -F3(copy, -1e9, -mx);
        if(t > mx){
            mx = t;
            best_move = moves[i];
        }
    }
    return best_move;
}


#endif // ALPHABETA_CPP