#ifndef MCTS_CPP
#define MCTS_CPP

#include "../h/mcts.h"
#include "../../lib/helper.h"
#include "../../lib/chess.h"
#include "../../lib/marisa.h"
#include <cmath>

long double UCB(int id, const std::vector<MCTSNode> &tree){
    const MCTSNode &node = tree[id];

    // standard win term
    long double ucb_value = (node.depth & 1) ? (node.Mean) : (1.0 - node.Mean);

    // amaf win term
    long double amafWinRate = (node.depth & 1) ? (node.AMAF_Mean) : (1.0 - node.AMAF_Mean);

    // get beta, i.e. sqrt(k / (3N + k))
    long double beta = 0.0;
    if(node.AMAF_N > 0){
        beta = sqrt(RAVE_EQUIV / (3.0 * node.sqrtN + RAVE_EQUIV));
    }

    // RAVE
    long double combinedMean = (1.0 - beta) * ucb_value + beta * amafWinRate;

    // exploration term
    long double exploration = tree[node.p_id].CsqrtlogN / node.sqrtN;

    return combinedMean + exploration;
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

int pos_simulate(Position &pos, std::vector<Move>&history, Move (*strategy)(MoveList<> &moves)){
    Position copy(pos);
    while(copy.winner() == NO_COLOR){
        MoveList<> moves(copy);
        Move m = strategy(moves);
        copy.do_move(m);
        history.push_back(m);
    }

    if(copy.winner() == copy.due_up()){
        return 1;
    }
    else if(copy.winner() == Mystery){
        return 0;
    }
    return -1;
}

SimResult simulate(Position &pos, int cur_id, std::vector<MCTSNode> &tree) {
    SimResult result;
    result.data = {0, 0, 0};
    
    for(int i = 0; i < tree[cur_id].Nchild; i++){
        int child_id = tree[cur_id].c_id[i];
        for(int j = 0; j < SIMULATION_PER_CHILD; j++){
            int win = pos_simulate(pos, result.moves_played, strategy_random);
            int actual_win = (tree[child_id].depth & 1) ? win : -win;
            result.data.deltaS += actual_win;
            result.data.deltaS2 += actual_win * actual_win;
            result.data.deltaN += 1;
            
        }
        // update child node
        UpdateData dummy_data = {0, 0, 0};
        update(child_id, dummy_data, tree);
    }
    return result;
}

bool was_played(const Move &m, const std::vector<Move> &moves_played){
    for(const Move &mv : moves_played){
        if(mv == m)
            return true;
    }
    return false;
}

void backpropagate(int id, const SimResult &data, std::vector<MCTSNode> &tree){
    int current_id = id;
    while(true){
        // standard update
        update(current_id, data.data, tree);

        // AMAF/RAVE update
        for(int i = 0; i < tree[current_id].Nchild; i++){
            Move move = tree[tree[current_id].c_id[i]].ply;
            if(was_played(move, data.moves_played)){
                tree[current_id].AMAF_N += data.data.deltaN;
                tree[current_id].AMAF_sum += data.data.deltaS;

                if(tree[current_id].AMAF_N > 0){
                    tree[current_id].AMAF_Mean = (long double) tree[current_id].AMAF_sum
                    / (long double) tree[current_id].AMAF_N;
                }
            }
        }

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

 SimResult terminal_update(int id, const Position &pos, std::vector<MCTSNode> &tree){
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

    SimResult res;
    res.data.deltaS = result;
    res.data.deltaS2 = result * result;
    res.data.deltaN = 1;
    // moves_played is empty
    return res;
}
#endif // MCTS_CPP
