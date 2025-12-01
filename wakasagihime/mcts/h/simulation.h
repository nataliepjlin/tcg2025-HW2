#ifndef SIMULATION_H
#define SIMULATION_H

#include "../../lib/types.h"
#include "../../lib/movegen.h"
#include "../../lib/chess.h"
#include "../../lib/helper.h"
#include <vector>
#include "node.h"
#include <algorithm>


const int yummy_table[7][8] = {
    // Victim:  Gen, Adv, Ele, Cha, Hor, Can, Sol | walk
    // ---------------------------------------------
    
    // general
    { 500, 300, 100,  30,  10,  30,   0, 10}, 

    // advisor
    {   0, 300, 100,  30,  10,  30,   5, 10},

    // elephant
    {   0,   0, 100,  30,  10,  30,   5, 12},

    // chariot
    {   0,   0,   0,  50,  20,  50,   5, 20},

    // horse
    {   0,   0,   0,   0,  20,  50,   5, 15},
    // cannon
    { 900, 300, 100,  50,  20,  50,   5, 20},

    // solider
    { 1000,  0,   0,   0,   0,   0,  10, 15} 
};

const int win_score = 16;

const int total_type = 14; // 0~6 even depth, 7~13 odd depth

int move_evaluation(const Position &pos, const Move &m);
Move strategy_weighted_random(const Position &pos, MoveList<> &moves);
int pos_simulation(Position &pos, long long played_moves[total_type][SQUARE_NB][SQUARE_NB], const Color root_color, const long long iter, long long cur_depth);

#endif // SIMULATION_H