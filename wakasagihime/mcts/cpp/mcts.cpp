#ifndef MCTS_CPP
#define MCTS_CPP

#include "../headers/mcts.h"
#include "../../lib/helper.h"
#include <math.h>
#include <limits.h>

const int MAX_SIMULATIONS = 10000;


MCTS::MCTS(Color c, Position pos, double c_const, int simulation):
    player_color(c),
    simulations_per_move(simulation),
    C(c_const)
{
    Nodes = std::make_unique<Node[]>(MaxNodes);
    nodes_tail = 0;
    root = pos;
    root_id = create_root(player_color);
}

void MCTS::MCTS_simulate(int simulations, double time_limit){
    for(int i = 0; i < simulations; i++){
        if(MCTS_iteration()){
            break;
        }
    }
}

bool MCTS::MCTS_iteration(){
    auto pv = find_pv();
    int node_id = pv.first;
    Position pos = pv.second;

    if(pos.winner() != NO_COLOR){
        // terminal node
        return true;
    }

    expand(node_id, pos);
    for(int i = 0; i < Nodes[node_id].Nchild; i++){
        Position child_pos(pos);
        child_pos.do_move(Nodes[node_id].moves[i]);
        int child_id = Nodes[node_id].c_id[i];
        int score = inner_simulate(child_pos, simulations_per_move);
        back_propagation(child_id, score, simulations_per_move);
    }
    return false;
}

Move MCTS::get_best_move(double time_limit, int simulations){
    MCTS_simulate(simulations, time_limit);
    int best_child = select_expandable(root_id);
    return Nodes[root_id].moves[best_child];
}

void MCTS::reset(Position pos, Color c){
    player_color = c;
    root = pos;
    nodes_tail = 0;
    root_id = create_root(player_color);
}

std::pair<int, Position> MCTS::find_pv(){
    int cur = root_id;
    Position pos(root);

    while(Nodes[cur].expandable == false){
        int next = select_expandable(cur);
        pos.do_move(Nodes[cur].moves[next]);
        cur = Nodes[cur].c_id[next];
    }
    return std::make_pair(cur, pos);
}

long double MCTS::ucb(int node_id, int pid){
    if(Nodes[node_id].Ntotal >= MAX_SIMULATIONS){
        return -Nodes[node_id].Mean;
    }
    if(Nodes[node_id].sqrtN == 0){
        return -LONG_MAX;
    }

    return -Nodes[node_id].Mean + Nodes[pid].CsqrtlogN / Nodes[node_id].sqrtN;
}

int MCTS::select_expandable(int node_id){
    int best = 0;
    long double bestScore = ucb(Nodes[node_id].c_id[0], node_id);

    for(int i = 1; i < Nodes[node_id].Nchild; i++){
        long double score = ucb(Nodes[node_id].c_id[i], node_id);
        if(bestScore < score){
            bestScore = score;
            best = i;
        }
    }
    return best;
}

void MCTS::expand(int node_id, Position pos){
    MoveList<> moves(pos);
    Nodes[node_id].Nchild = 0;
    for(int i = 0; i < moves.size(); i++){
        Position child_pos(pos);
        if(!child_pos.do_move(moves[i])){
            continue;
        }
        int child_id = create_node(node_id);
        Nodes[node_id].c_id[i] = child_id;
        Nodes[node_id].moves[i] = moves[i];
        Nodes[node_id].Nchild++;
    }
    Nodes[node_id].expandable = false;
}

int MCTS::inner_simulate(Position pos, int simulations){
    int local_score = 0;
    for (int j = 0; j < simulations; j += 1) {
        local_score += pos.simulate(strategy_random);
    }
    return local_score;
}

void MCTS::update(int node_id, long long score, int simulations){
    Node &node = Nodes[node_id];
    node.Ntotal += simulations;
    node.sum1 += score;
    node.sum2 += (score * score);
    node.Mean = (long double)(node.sum1) / node.Ntotal;
    node.Variance = ((long double)(node.sum2) / node.Ntotal) - (node.Mean * node.Mean);
    node.sqrtN = sqrt(node.Ntotal);
    node.CsqrtlogN = C * sqrt(log(node.Ntotal));
}

void MCTS::back_propagation(int node_id, long long score, int simulations){
    int cur = node_id;
    update(cur, score, simulations);
    while(Nodes[cur].p_id != cur){
        score = -score;
        cur = Nodes[cur].p_id;
        update(cur, score, simulations);
    }
}

#endif