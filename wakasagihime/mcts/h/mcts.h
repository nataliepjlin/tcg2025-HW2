#ifndef MCTS_H
#define MCTS_H

#include <vector>
#include "node.h"
#include "simulation.h"

const int SIMULATION_PER_CHILD = 200;
const long double C = 1.8L;
const int root_id = 0;
const int inf = 1e9;
const int MAX_VISIT = 1e4;
const long double RANGE = 2.0L;
const long double MIN_S = -1.0L;

const double RAVE_EQUIV = 10000.0;

long double UCB(int id, const std::vector<MCTSNode> &tree);
int find_best_ucb(int cur_id, const std::vector<MCTSNode> &tree);
Position find_pv(const Position &pos, int &cur_id, const std::vector<MCTSNode> &tree);
bool expand(const Position &pos, const int cur_id, std::vector<MCTSNode> &tree);
void update(int id, std::vector<MCTSNode> &tree);
void mcts_simulate(Position &pos, int cur_id, std::vector<MCTSNode> &tree, const Color root_color);
void backpropagate(int id, int deltaS, int deltaS2, const int deltaN, std::vector<MCTSNode> &tree, const std::vector<AmafMove>& played_moves);
int find_best_move(const std::vector<MCTSNode> &tree);
void terminal_update(int id, const Position &pos, std::vector<MCTSNode> &tree, const Color root_color);
bool is_move_in_simulation(const MCTSNode &node, const std::vector<AmafMove> &played_moves);

#endif // MCTS_H