// Wakasagihime
// Plays Chinese Dark Chess (Banqi)!

#include "lib/chess.h"
#include "lib/marisa.h"
#include "lib/types.h"
#include "lib/helper.h"
#include <cmath>
#include <chrono>
#include "mcts/h/mcts.h"
#include <fstream>

// Girls are preparing...
__attribute__((constructor)) void prepare()
{
    // Prepare the distance table
    for (Square i = SQ_A1; i < SQUARE_NB; i += 1) {
        for (Square j = SQ_A1; j < SQUARE_NB; j += 1) {
            SquareDistance[i][j] = distance<Rank>(i, j) + distance<File>(i, j);
        }
    }

    // Prepare the attack table (regular)
    Direction dirs[4] = { NORTH, SOUTH, EAST, WEST };
    for (Square sq = SQ_A1; is_okay(sq); sq += 1) {
        Board a = 0;
        for (Direction d : dirs) {
            a |= safe_destination(sq, d);
        }
        PseudoAttacks[sq] = a;
    }

    // Prepare magic
    init_magic<Cannon>(cannonTable, cannonMagics);
}

void log_position(int best, const std::vector<MCTSNode>& nodes) {
    std::ofstream fout("./log.log", std::ios::app); // append mode
    if (!fout.is_open()) {
        std::cerr << "Failed to open log file\n";
        return;
    }


    fout << "Total simulations: " << nodes[0].Ntotal << "\n";
    fout << "Best simulations mean: " << nodes[best].Mean << "\n";
    fout << "------------------------\n";

    fout.close();
}

void show_tree(const std::vector<MCTSNode>& nodes) {
    int max_depth = 0;
    long int total_depth = 0;
    int leaf = 0;
    for (size_t i = 0; i < nodes.size(); ++i) {
        if(nodes[i].Nchild == 0) {
            ++leaf;
            total_depth += nodes[i].depth;
            if (nodes[i].depth > max_depth) {
                max_depth = nodes[i].depth;
            }
        }
    }
    std::ofstream fout("./log.log", std::ios::app); // append mode
    if (!fout.is_open()) {
        std::cerr << "Failed to open log file\n";
        return;
    }


    fout << "Max depth: " << max_depth << "\n";
    fout << "Average depth: " << (leaf > 0 ? static_cast<double>(total_depth) / leaf : 0) << "\n";
    fout << "Leaf nodes: " << leaf << "\n";
    fout << "------------------------\n";

    fout.close();
}

// le fishe
int main()
{
    std::string line;
    std::chrono::milliseconds TIME_LIMIT(4500);

    /* read input board state */
    while (std::getline(std::cin, line)) {
        auto start_time = std::chrono::steady_clock::now();
        Position pos(line);
        
        // build root node
        std::vector<MCTSNode> tree;
        tree.push_back(MCTSNode(0, 0)); // root

        // MCTS main loop
        while (std::chrono::steady_clock::now() - start_time < TIME_LIMIT){
            // Selection
            int current_id = root_id;
            Color root_color = pos.due_up();
            Position pv_pos = find_pv(pos, current_id, tree);// current_id is updated inside

            // Expansion
            if(!expand(pv_pos, current_id, tree)){
                // a terminal node is reached
                terminal_update(current_id, pv_pos, tree, root_color);
            }
            else{
                // Simulation & Backpropagation
                mcts_simulate(pv_pos, current_id, tree, root_color);
            }
        }

        // choose the best move
        int best_id = find_best_move(tree);
        info << tree[best_id].ply;
        log_position(best_id, tree);
        show_tree(tree);
    }
}
