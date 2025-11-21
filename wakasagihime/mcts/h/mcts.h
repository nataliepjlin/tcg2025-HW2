#ifndef MCTS_H
#define MCTS_H

#include <vector>
#include "node.h"

const int SIMULATION_PER_CHILD = 25;
const long double C = 1.4L;
const int root_id = 0;
struct UpdateData {
    int deltaS;
    int deltaS2;
    int deltaN;
};


long double UCB(int id, const std::vector<MCTSNode> &tree);
int find_best_ucb(int cur_id, const std::vector<MCTSNode> &tree);
Position find_pv(const Position &pos, int &cur_id, const std::vector<MCTSNode> &tree);
bool expand(const Position &pos, const int cur_id, std::vector<MCTSNode> &tree);
void update(int id, const UpdateData &data, std::vector<MCTSNode> &tree);
UpdateData simulate(Position &pos, int cur_id, std::vector<MCTSNode> &tree);
void backpropagate(int id, const UpdateData &data, std::vector<MCTSNode> &tree);
int find_best_move(const std::vector<MCTSNode> &tree);
UpdateData terminal_update(int id, const Position &pos, std::vector<MCTSNode> &tree);

#endif // MCTS_H