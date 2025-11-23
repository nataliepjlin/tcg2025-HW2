#ifndef SIMULATION_H
#define SIMULATION_H

#include "../../lib/types.h"
#include "../../lib/movegen.h"
#include "../../lib/chess.h"
#include "../../lib/helper.h"
#include <vector>
#include "node.h"
#include <algorithm>


const int yummy_table[7][7] = {
    // Victim:  Gen, Adv, Ele, Cha, Hor, Can, Sol
    // ---------------------------------------------
    
    // general
    { 50,  20,  15,  10,   8,  25,   0}, 

    // advisor
    {  0,  20,  15,  10,   8,  25,   4}, 

    // elephant
    {  0,   0,  15,  10,   8,  25,   4}, 

    // chariot
    {  0,   0,   0,  10,   8,  25,   4}, 

    // horse
    {  0,   0,   0,   0,   5,  25,   5}, 

    // cannon
    { 60,  30,  20,  15,  10,  30,   2}, 

    // solider
    { 200,   0,   0,   0,   0,   0,   5} 
};

const int other_move_score = 1;

const int AMAF_CUTOFF = 15; // Only record the first 15 moves of random play

struct AmafMove{
    Move m;
    PieceType pt_from;
    Color c_from;
};

int move_evaluation(const Position &pos, const Move &m);
Move strategy_weighted_random(const Position &pos, MoveList<> &moves);
int pos_simulation(Position &pos, std::vector<AmafMove> &played_moves);

#endif // SIMULATION_H