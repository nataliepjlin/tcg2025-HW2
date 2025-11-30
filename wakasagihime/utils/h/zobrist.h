#ifndef ZOBRIST_H
#define ZOBRIST_H

#include "../../lib/helper.h"
#include "../../lib/pcg-cpp-0.98/include/pcg_random.hpp"

void init_zobrist();
uint64_t compute_zobrist_hash(const Position &pos);

#endif // ZOBRIST_H