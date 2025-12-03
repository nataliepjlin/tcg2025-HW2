#ifndef MCTS_CPP
#define MCTS_CPP

#include "../h/mcts.h"
#include "../../lib/helper.h"
#include "../../lib/chess.h"
#include "../../lib/marisa.h"
#include <cmath>
#include <limits>

long double UCB(int id, const std::vector<MCTSNode> &tree){
    if(tree[id].sqrtN == 0) return inf; 
    
    // mcts ucb
    long double mc_score = (tree[id].depth & 1) ? (1 - (tree[id].Mean-MIN_S) / RANGE) : (tree[id].Mean - MIN_S) / RANGE;
    
    // If visited enough, purely exploit
    if(tree[id].Ntotal >= MAX_VISIT)
        return mc_score;
    // amaf
    long double amaf_score = 0;
    if(tree[id].N_AMAF > 0){
        amaf_score = (tree[id].depth & 1) ? (1 - (tree[id].Mean_AMAF - MIN_S) / RANGE) : (tree[id].Mean_AMAF - MIN_S) / RANGE;
    }

    long double alpha = std::min(1.0L, (long double)tree[id].Ntotal / RAVE_EQUIV);
    long double combined_score = alpha * mc_score + (1.0L - alpha) * amaf_score;

    return combined_score + tree[ tree[id].p_id ].CsqrtlogN / tree[id].sqrtN;
}

int find_best_ucb(int cur_id, const std::vector<MCTSNode> &tree){
    int maxchild = tree[cur_id].c_id[0];
    long double maxV = UCB(maxchild, tree);
    for(int i = 1; i < tree[cur_id].Nchild; i++){
        int ctemp = tree[cur_id].c_id[i];
        long double temp = UCB(ctemp, tree);
        if(maxV < temp){
            maxV = temp; maxchild = ctemp;
        }
    }
    return maxchild;
}

Position find_pv(const Position &pos, int &cur_id, const std::vector<MCTSNode> &tree){

    Position pv_pos(pos);

    while(tree[cur_id].Nchild > 0){ // while not reaching a leaf
        int next_id = find_best_ucb(cur_id, tree);
        pv_pos.do_move(tree[next_id].ply);
        cur_id = next_id;
    }
    return pv_pos;
}

bool expand(const Position &pos, const int cur_id, std::vector<MCTSNode> &tree){
    MoveList<> moves(pos);

    if(moves.size() == 0)
        return false; // no expansion possible

    for(int i = 0; i < moves.size(); i++){
        tree.push_back(MCTSNode(cur_id, tree[cur_id].depth + 1, moves[i], pos));
        tree[cur_id].Nchild++;
        tree[cur_id].c_id[tree[cur_id].Nchild - 1] = tree.size() - 1;
    }
    return true;
}

void update(int id, const int deltaS, const int deltaS2, const int deltaN, std::vector<MCTSNode> &tree){
    tree[id].Ntotal += deltaN; 
    tree[id].CsqrtlogN = C * sqrt(log((long double) tree[id].Ntotal));
    tree[id].sqrtN = sqrt((long double) tree[id].Ntotal);
    tree[id].sum1 += deltaS; 
    tree[id].sum2 += deltaS2;
    tree[id].Mean = (long double) tree[id].sum1 / (long double) tree[id].Ntotal;
    
    tree[id].Variance = (long double) tree[id].sum2 / (long double) tree[id].Ntotal -
                        tree[id].Mean * tree[id].Mean;
}

bool early_termination_checker(const Position &pos, const std::vector<Square> &pcs1, const std::vector<Square> &pcs2){
    for(Square rsq: pcs1){
        bool can_capture_all = true;
        PieceType my_pt = pos.peek_piece_at(rsq).type;
        
        for(Square bsq: pcs2){
            PieceType op_pt = pos.peek_piece_at(bsq).type;
            
            if(!(my_pt > op_pt)){ 
                can_capture_all = false;
                break;
            }
        }
        if(can_capture_all) return true;
    }
    return false;
}

bool early_termination(Position &pos){

    // the opponent has no cannon and you have a piece that can capture all of your opponent’s pieces.
    std::vector<Square> red_pcs = squares_sorted(pos, pos.pieces(Red));
    std::vector<Square> black_pcs = squares_sorted(pos, pos.pieces(Black));

    bool has_cannon_red = pos.count(Red, Cannon) > 0 && pos.count(Red) > 1;// more than just cannon
    bool has_cannon_black = pos.count(Black, Cannon) > 0 && pos.count(Black) > 1;
    
    return (!has_cannon_black &&
            early_termination_checker(pos, red_pcs, black_pcs)) ||
           (!has_cannon_red &&
            early_termination_checker(pos, black_pcs, red_pcs));
}

void mcts_simulate(Position &pos, int cur_id, std::vector<MCTSNode> &tree, const Color root_color){
    long long played_moves[total_type][SQUARE_NB][SQUARE_NB] = {{{0}}};// [piece_type][from][to]

    long long iter = 1;
    for(int i = 0; i < tree[cur_id].Nchild; i++){
        int child_id = tree[cur_id].c_id[i];
        for(int j = 0; j < INITIAL_SIMULATIONS; j++){
            Position copy(pos);
            copy.do_move(tree[child_id].ply);
            int result = pos_simulation(copy, played_moves, root_color, iter, tree[child_id].depth);
            backpropagate(child_id, result, result * result, 1, tree, played_moves);
            iter++;
        }
    }

    for(int i = 0; i < SIMULATION_PER_ACTION; i++){
        int best_child = find_best_ucb(cur_id, tree);
        Position copy(pos);
        copy.do_move(tree[best_child].ply);
        int result = pos_simulation(copy, played_moves, root_color, iter, tree[best_child].depth);
        backpropagate(best_child, result, result * result, 1, tree, played_moves);
        iter++;
    }
}

bool is_move_in_simulation(const MCTSNode &node, const long long played_moves[total_type][SQUARE_NB][SQUARE_NB], const long long iter){
    int type_index = (node.depth & 1) ? (7 + node.pt_from) : node.pt_from;
    return played_moves[type_index][node.ply.from()][node.ply.to()] == iter;
}

void backpropagate(int id, int deltaS, int deltaS2, const int deltaN, std::vector<MCTSNode> &tree, const long long played_moves[total_type][SQUARE_NB][SQUARE_NB]){
    int current_id = id;
    while(true){
        // standard mcts update
        update(current_id, deltaS, deltaS2, deltaN, tree);
        if(current_id == root_id)
            break;

        // amaf update
        int parent_id = tree[current_id].p_id;
        if(played_moves != nullptr){// not a terminal update
            for(int i = 0; i < tree[parent_id].Nchild; i++){
                int sibling_id = tree[parent_id].c_id[i];
                if(sibling_id == current_id)
                    continue;
                
                if(is_move_in_simulation(tree[sibling_id], played_moves, tree[sibling_id].depth)){
                    tree[sibling_id].N_AMAF += deltaN;
                    tree[sibling_id].sum1_AMAF += deltaS;
                    tree[sibling_id].Mean_AMAF = (long double)tree[sibling_id].sum1_AMAF / tree[sibling_id].N_AMAF;
                }
            }
        }

        current_id = parent_id;
    }
}

int find_best_move(const std::vector<MCTSNode> &tree){
    if(tree[root_id].Nchild == 0) {
        // No children expanded, this should not happen but safety check
        return -1;
    }
    
    int best_id = tree[root_id].c_id[0];
    long double bestWR = tree[best_id].Mean; 
    
    for(int i = 1; i < tree[root_id].Nchild; i++){
        int ctemp = tree[root_id].c_id[i];
        
        long double tempWR = tree[ctemp].Mean; 
        
        if(bestWR < tempWR){
            bestWR = tempWR; best_id = ctemp;
        }
    }
    return best_id;
}

void terminal_update(int id, const Position &pos, std::vector<MCTSNode> &tree, const Color root_color){
    int result;
    int diff = pos.count(root_color) - pos.count(Color(root_color ^ 1));
    if(pos.winner() == root_color){
        result = win_score + diff;
    }
    else if(pos.winner() == Mystery){
        result = diff;
    }
    else{
        result = -win_score + diff;
    }
    backpropagate(id, result, result * result, 1, tree, {});
}
#endif // MCTS_CPP
