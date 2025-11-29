#ifndef EVAL_CPP
#define EVAL_CPP

#include "../h/eval.h"

bool is_new_game(const Position &pos){
    return pos.count() == SQUARE_NB;
}
#endif // EVAL_CPP