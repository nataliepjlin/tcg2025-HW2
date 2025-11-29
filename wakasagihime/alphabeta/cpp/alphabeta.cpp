#ifndef ALPHABETA_CPP
#define ALPHABETA_CPP

#include "../h/alphabeta.h"

// Global time check
std::chrono::time_point<std::chrono::steady_clock> ab_start_time;
const int AB_TIME_LIMIT_MS = 4500;
bool time_out = false;

bool is_terminal(Position &pos){
    return pos.winner() != NO_COLOR;
}

bool move_compare(const Position &pos, const Move &a, const Move &b){
    return move_evaluation(pos, a) > move_evaluation(pos, b);
}

void log_alphabeta(int depth){
    std::ofstream fout("./log.log", std::ios::app); // append mode
    if (!fout.is_open()) {
        std::cerr << "Failed to open log file\n";
        return;
    }

    fout << "Alpha-Beta search used:\n";
    fout << "Time used (ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - ab_start_time).count() << "\n";
    fout << "depth used: " << depth << "\n";
    fout << "------------------------\n";

    fout.close();
}

int F3(Position &pos, int alpha, int beta, int depth){
    // Check time every 1024 nodes (bitwise AND is faster than modulo)
    static int node_count = 0;
    if ((++node_count & 1023) == 0){
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - ab_start_time).count() > AB_TIME_LIMIT_MS){
            time_out = true;
            return 0;
        }
    }
    if(time_out) return 0;

    // Depth limit check
    if(is_terminal(pos)){
        int diff = pos_score(pos, pos.due_up());
        // FIX: Add depth bonus. Higher depth value = shallower in tree (closer to root) = faster win.
        // Assuming depth counts DOWN from Max to 0.
        if(pos.winner() == pos.due_up())
            return ab_win_score + depth + diff; 
        else if(pos.winner() == Mystery)
            return diff; // Draw
        else
            return -(ab_win_score + depth) + diff;
    }

    // Depth Cutoff
    if(depth == 0) {
        return pos_score(pos, pos.due_up());
    }

    MoveList<> moves(pos);
    std::sort(moves.begin(), moves.end(), [&](const Move &a, const Move &b){
        return move_compare(pos, a, b);
    });

    int mx = -2e9;
    bool has_safe_move = false;

    for(int i = 0; i < moves.size(); i++){
        Position copy(pos);
        copy.do_move(moves[i]);
        
        int t = -F3(copy, -beta, -std::max(alpha, mx), depth - 1);
        
        if (time_out) return 0;

        if(t > mx){
            mx = t;
            has_safe_move = true;
        }
        if(mx >= beta)
            return mx;
    }
    
    // Stalemate check
    if (!has_safe_move) return -(ab_win_score + depth); 

    return mx;
}

Move alphabeta_search(Position &pos){
    ab_start_time = std::chrono::steady_clock::now();
    time_out = false;

    MoveList<> moves(pos);
    // Initial sort
    std::sort(moves.begin(), moves.end(), [&](const Move &a, const Move &b){
        return move_compare(pos, a, b);
    });

    Move best_move = moves[0];
    Move best_move_this_iter = moves[0];
    
    // 3. Iterative Deepening Loop
    // Start at depth 1, increase until time runs out
    int depth = 1;
    for (; depth <= 50; depth++){
        
        int mx = -2e9;
        int alpha = -2e9;
        int beta = 2e9;

        // Search root children manually to track best_move
        for(int i = 0; i < moves.size(); i++){
            Position copy(pos);
            copy.do_move(moves[i]);
            
            // Call F3 with depth - 1
            int t = -F3(copy, -beta, -std::max(alpha, mx), depth - 1);

            if (time_out) break; // Break inner loop

            if(t > mx){
                mx = t;
                best_move_this_iter = moves[i];
            }
        }

        if (time_out){
            // If we timed out during Depth K, the results are incomplete/garbage.
            // We MUST discard Depth K and return the result from Depth K-1.
            break; 
        }
        else{
            best_move = best_move_this_iter;
            // Optimization: If we found a forced mate, stop early
            if (mx > ab_win_score - 5000) break;
        }
    }
    log_alphabeta(depth - 1);
    return best_move;
}

int pos_score(Position &pos, const Color cur_color){
    int score = 0;
    Color opp_color = Color(cur_color ^ 1);
    
    for(Square sq = SQ_A1; sq < SQUARE_NB; sq = Square(sq + 1)){
        Piece p = pos.peek_piece_at(sq);
        if(p.side == cur_color){
            score += Piece_Value[p.type];
        }
        else if(p.side == opp_color){
            score -= Piece_Value[p.type];
        }
    }

    Board my_board = pos.pieces(cur_color);
    Board opp_board = pos.pieces(opp_color);
    if(pos.count(cur_color) && pos.count(opp_color)){
        int min_dist = 1000;
        
        for(Square my_sq : BoardView(my_board)){
            for(Square opp_sq : BoardView(opp_board)){
                int dist = SquareDistance[my_sq][opp_sq];
                if(dist < min_dist){
                    min_dist = dist;
                }
            }
        }
        // Subtract distance from score. 
        // Closer = Smaller Distance = Less Penalty = Higher Score.
        score -= min_dist; 
    }

    return score;
}

#endif // ALPHABETA_CPP