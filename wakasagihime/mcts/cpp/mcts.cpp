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
    
    // If visited enough, purely exploit
    if(tree[id].Ntotal >= MAX_VISIT)
        return -tree[id].Mean;

    long double exploitation = -tree[id].Mean; 
    return exploitation + tree[ tree[id].p_id ].CsqrtlogN / tree[id].sqrtN;
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
        tree.push_back(MCTSNode(cur_id, tree[cur_id].depth + 1, moves[i]));
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

void simulate(Position &pos, int cur_id, std::vector<MCTSNode> &tree){
    for(int i = 0; i < tree[cur_id].Nchild; i++){
        int child_id = tree[cur_id].c_id[i];
        for(int j = 0; j < SIMULATION_PER_CHILD; j++){
            Position copy(pos);
            copy.do_move(tree[child_id].ply);
            int result = copy.simulate(strategy_random);

            backpropagate(child_id, result, result * result, 1, tree);
        }
    }
}

void backpropagate(int id, int deltaS, int deltaS2, const int deltaN, std::vector<MCTSNode> &tree){
    int current_id = id;
    while(true){
        update(current_id, deltaS, deltaS2, deltaN, tree);
        if(current_id == root_id)
            break;
        current_id = tree[current_id].p_id;
        deltaS = -deltaS;// switch perspective
    }
}

int find_best_move(const std::vector<MCTSNode> &tree){
    
    int best_id = tree[root_id].c_id[0];long double bestWR = -tree[best_id].Mean; 
    
    for(int i = 1; i < tree[root_id].Nchild; i++){
        int ctemp = tree[root_id].c_id[i];
        
        long double tempWR = -tree[ctemp].Mean; 
        
        if(bestWR < tempWR){
            bestWR = tempWR; best_id = ctemp;
        }
    }
    return best_id;
}

void terminal_update(int id, const Position &pos, std::vector<MCTSNode> &tree){
    int result;
    if(pos.winner() == pos.due_up()){
        result = 1;
    }
    else if(pos.winner() == Mystery){
        result = 0;
    }
    else{
        result = -1;
    }
    backpropagate(id, result, result * result, 1, tree);
}
#endif // MCTS_CPP
