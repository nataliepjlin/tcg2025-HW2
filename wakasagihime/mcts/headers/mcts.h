#ifndef MCTS_H
#define MCTS_H
#include "node.h"
class MCTS{
public:
    MCTS(Color c, Position pos, double c_const = 1.8, int simulate = 5);
    void MCTS_simulate(int simulations, double time_limit = 4.9);
    bool MCTS_iteration();
    Move get_best_move(double time_limit = 4.9, int simulations = 0);
    void reset(Position pos, Color c);

    Color player_color;
    int simulations_per_move;


    Position root;
    std::unique_ptr<Node[]> Nodes;
    int nodes_tail;

    int create_root(const Color &c){
        int root_idx = nodes_tail;
        Nodes[nodes_tail] = Node(c, root_idx);  // Root's parent is itself
        nodes_tail += 1;
        return root_idx;
    }
    int create_node(int pid){
        Nodes[nodes_tail] = Node(pid, Nodes[pid]);
        nodes_tail += 1;
        return (nodes_tail - 1);
    }

    int root_id;
    double C; // exploration constant

    std::pair<int, Position> find_pv();

    int select_expandable(int node_id);

    void expand(int node_id, Position pos);

    int inner_simulate(Position pos, int simulations);

    void update(int node_id, long long score, int simulations);
    void back_propagation(int node_id, long long score, int simulations);

    long double ucb(int node_id, int pid);
};
#endif