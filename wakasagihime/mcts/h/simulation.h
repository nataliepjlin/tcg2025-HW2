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
    { 200, 90,  80,  70,  60,  85,   0, 20}, 

    // advisor
    {  0,  90,  70,  60,  50,  75,  20, 16},

    // elephant
    {  0,   0,  80, 50,  40,  70,   16, 12},

    // chariot
    {  0,   0,   0,  70, 30,  70,   12, 8},

    // horse
    {  0,   0,   0,   0,  60, 70, 8, 4},
    // cannon
    { 800,  90,  75,  55,  40,  85, 8, 12},

    // solider
    { 1000,   0,   0,   0,   0,   0, 8, 1} 
};

const int win_score = 16;

const int total_type = 14; // 0~6 even depth, 7~13 odd depth

int move_evaluation(const Position &pos, const Move &m);
Move strategy_weighted_random(const Position &pos, MoveList<> &moves);
int pos_simulation(Position &pos, int played_moves[total_type][SQUARE_NB][SQUARE_NB], const Color root_color, const int iter, int cur_depth);

#endif // SIMULATION_H