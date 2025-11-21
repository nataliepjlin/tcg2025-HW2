// Wakasagihime
// Plays Chinese Dark Chess (Banqi)!

#include "lib/chess.h"
#include "lib/marisa.h"
#include "lib/types.h"
#include "lib/helper.h"
#include <cmath>
#include <chrono>
#include "mcts/h/mcts.h"

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

// le fishe
int main()
{
    std::string line;
    std::chrono::milliseconds TIME_LIMIT(4900);
    auto start_time = std::chrono::steady_clock::now();

    /* read input board state */
    while (std::getline(std::cin, line)) {
        Position pos(line);
        
        // build root node
        std::vector<MCTSNode> tree;
        tree.push_back(MCTSNode(0, 0)); // root

        // MCTS main loop
        while (std::chrono::steady_clock::now() - start_time < TIME_LIMIT){
            // Selection
            int current_id = root_id;
            Position pv_pos = find_pv(pos, current_id, tree);// current_id is updated inside

            SimResult res;
            // Expansion
            if(!expand(pv_pos, current_id, tree)){
                // a terminal node is reached
                res = terminal_update(current_id, pv_pos, tree);
            }

            // Simulation
            res = simulate(pv_pos, current_id, tree);// retrieve data to update pv
            
            // Backpropagation
            backpropagate(current_id, res, tree);
        }

        // choose the best move
        int best_id = find_best_move(tree);
        info << tree[best_id].ply;
    }
}
