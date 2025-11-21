#ifndef MCTS_CPP
#define MCTS_CPP

#include "../h/mcts.h"
#include "../../lib/helper.h"
#include "../../lib/chess.h"
#include "../../lib/marisa.h"
#include <cmath>

long double UCB(int id, const std::vector<MCTSNode> &tree){
    long double exploitation = (tree[id].depth & 1) ? (tree[id].Mean) : (1.0 - tree[id].Mean);
    return exploitation + tree[ tree[id].p_id ].CsqrtlogN/tree[id].sqrtN;
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

void update(int id, const UpdateData &data, std::vector<MCTSNode> &tree){
    tree[id].Ntotal += data.deltaN; // additional # of trials
    tree[id].CsqrtlogN = C * sqrt(log((long double) tree[id].Ntotal));
    tree[id].sqrtN = sqrt((long double) tree[id].Ntotal);
    tree[id].sum1 += data.deltaS; // additional scores in trials
    tree[id].sum2 += data.deltaS2;
    tree[id].Mean = (long double) tree[id].sum1
    / (long double) tree[id].Ntotal;
    tree[id].Variance = (long double) tree[id].sum2
    / (long double) tree[id].Ntotal -
    tree[id].Mean * tree[id].Mean;
}

UpdateData simulate(Position &pos, int cur_id, std::vector<MCTSNode> &tree){// returns number of extra visits
    
    UpdateData data = {0, 0, 0};
    UpdateData dummy_data = {0, 0, 0};// to update leaf node later
    
    for(int i = 0; i < tree[cur_id].Nchild; i++){
        int child_id = tree[cur_id].c_id[i];
        for(int j = 0; j < SIMULATION_PER_CHILD; j++){
            Position copy(pos);
            copy.do_move(tree[child_id].ply);
            int result = copy.simulate(strategy_random);
            result = (result == -1); // convert to win for side to move

            tree[child_id].sum1 += result;
            tree[child_id].sum2 += result * result;
            tree[child_id].Ntotal++;

            data.deltaS += result;
            data.deltaS2 += result * result;
            data.deltaN++;
        }
        // update child node
        update(child_id, dummy_data, tree);
    }
    return data;
}

void backpropagate(int id, const UpdateData &data, std::vector<MCTSNode> &tree){
    int current_id = id;
    while(true){
        update(current_id, data, tree);
        if(current_id == root_id)
            break;
        current_id = tree[current_id].p_id;
    }
}

int find_best_move(const std::vector<MCTSNode> &tree){
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

UpdateData terminal_update(int id, const Position &pos, std::vector<MCTSNode> &tree){
    // update terminal node with win/loss
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

    UpdateData data;
    data.deltaS = result;
    data.deltaS2 = result * result;
    data.deltaN = 1;
    return data;
}
#endif // MCTS_CPP
