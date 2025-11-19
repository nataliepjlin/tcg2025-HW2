// Wakasagihime
// Plays Chinese Dark Chess (Banqi)!

#include "lib/chess.h"
#include "lib/marisa.h"
#include "lib/types.h"
#include "lib/helper.h"
#include <cmath>
#include <chrono>
#include <vector>
#include <limits>

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

// UCB node structure for storing statistics
struct UCBNode{
    Move move;
    int wins = 0;
    int visits = 0;
    double sqrt_visits = 0.0;
    double ucb_score = 0.0;
    
    void update_ucb(int total_visits, double c = 1.414) {
        if(visits == 0){
            ucb_score = std::numeric_limits<double>::max();
        }
        else{
            double win_rate = (double)wins / visits;
            double exploration = c * sqrt(log(total_visits) / visits);
            ucb_score = win_rate + exploration;
        }
    }
};

// le fishe
int main()
{
    std::string line;
    const int SIMULATION_PER_CHILD = 25;  // Initial simulations per child
    const int ADDITIONAL_SIMULATION = 3;  // Additional simulations per iteration
    std::chrono::milliseconds TIME_LIMIT(4900);
    auto start_time = std::chrono::steady_clock::now();

    /* read input board state */
    while (std::getline(std::cin, line)) {
        Position pos(line);
        MoveList<> moves(pos);
        
        if(moves.size() == 0){
            continue; // No legal moves
        }
        
        if(moves.size() == 1){
            // Only one move available
            info << moves[0];
            continue;
        }
        
        std::vector<UCBNode> children(moves.size());
        
        int total_visits = 0;
        for(int i = 0; i < moves.size(); i++){
            children[i].move = moves[i];

            for(int j = 0; j < SIMULATION_PER_CHILD; j++){
                Position copy(pos);
                copy.do_move(moves[i]);
                int result = copy.simulate(strategy_random);
                
                children[i].wins += (result == -1);
                children[i].visits++;
                total_visits++;
            }
        }
        
        // Calculate initial UCB scores
        for(auto& child : children){
            child.update_ucb(total_visits);
        }
        
        // while there is still time
        while(std::chrono::steady_clock::now() - start_time < TIME_LIMIT){
            // Pick child with largest UCB score
            int best_idx = 0;
            double best_ucb = children[0].ucb_score;
            for(int i = 1; i < children.size(); i++){
                if(children[i].ucb_score > best_ucb){
                    best_ucb = children[i].ucb_score;
                    best_idx = i;
                }
                else if(children[i].ucb_score == best_ucb){
                    if(children[i].visits < children[best_idx].visits){
                        best_idx = i;
                    }
                }
            }
            
            for(int j = 0; j < ADDITIONAL_SIMULATION; j++) {
                Position copy(pos);
                copy.do_move(children[best_idx].move);
                int result = copy.simulate(strategy_random);
                
                // Update statistics
                children[best_idx].wins += (result == -1);
                children[best_idx].visits++;
                total_visits++;
            }
            
            // Update UCB scores for all children
            for(auto& child : children){
                child.update_ucb(total_visits);
            }
        }
        
        // Pick child with the largest winning rate to play
        int best_move_idx = 0;
        double best_win_rate = (double)children[0].wins / children[0].visits;
        
        for (int i = 1; i < children.size(); i++) {
            double win_rate = (double)children[i].wins / children[i].visits;
            if(win_rate > best_win_rate){
                best_win_rate = win_rate;
                best_move_idx = i;
            }
            else if(win_rate == best_win_rate){
                if(children[i].visits < children[best_move_idx].visits){
                    best_move_idx = i;
                }
            }
        }
        
        
        info << children[best_move_idx].move;
    }
}
