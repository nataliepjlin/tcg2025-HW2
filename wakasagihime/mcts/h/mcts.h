#ifndef MCTS_H
#define MCTS_H

#include <vector>
#include "node.h"

const int SIMULATION_PER_CHILD = 1;
const long double C = 1.4L;
const int root_id = 0;
const double RAVE_EQUIV = 1000.0;// need to tune

struct UpdateData {
    int deltaS;
    int deltaS2;
    int deltaN;
};

struct SimResult {
    UpdateData data;
    std::vector<Move> moves_played; 
};

long double UCB(int id, const std::vector<MCTSNode> &tree);
int find_best_ucb(int cur_id, const std::vector<MCTSNode> &tree);
Position find_pv(const Position &pos, int &cur_id, const std::vector<MCTSNode> &tree);
bool expand(const Position &pos, const int cur_id, std::vector<MCTSNode> &tree);
void update(int id, const UpdateData &data, std::vector<MCTSNode> &tree);
SimResult simulate(Position &pos, int cur_id, std::vector<MCTSNode> &tree);
bool was_played(const Move &m, const std::vector<Move> &moves_played);
void backpropagate(int id, const SimResult &data, std::vector<MCTSNode> &tree);
int find_best_move(const std::vector<MCTSNode> &tree);
SimResult terminal_update(int id, const Position &pos, std::vector<MCTSNode> &tree);

#endif // MCTS_H