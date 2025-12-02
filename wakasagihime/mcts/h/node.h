#ifndef NODE_H
#define NODE_H

#include <vector>
#include "../../lib/chess.h"
const int MaxChild = 100;

class MCTSNode{
public:
    Move ply;  // the ply from parent to here
    long long p_id; // parent id, root’s parent is the root
    long long c_id[MaxChild]; // children id
    int depth; // depth, 0 for the root
    long long Nchild; // number of children
    long long Ntotal; // total # of simulations
    long double CsqrtlogN; // c * sqrt(log(Ntotal))
    long double sqrtN; // sqrt(Ntotal)
    long long sum1; // sum1: sum of scores
    long long sum2; // sum2: sum of square of each score
    long double Mean; // average score, i.e. win rate
    long double Variance; // variance of score

    Color c_from;
    PieceType pt_from;

    int N_AMAF;
    long long sum1_AMAF;
    long double Mean_AMAF;

    // root
    MCTSNode(long long pid = 0, long long d = 0){
        p_id = pid;
        depth = d;
        // expandable = true;
        Nchild = 0;
        Ntotal = 0;

        sum1 = 0;
        sum2 = 0;

        N_AMAF = 0;
        sum1_AMAF = 0;
        Mean_AMAF = 0;
    }

    // non-root
    MCTSNode(long long pid, long long d, Move m, const Position &pos){
        p_id = pid;
        depth = d;
        ply = m;
        Nchild = 0;
        Ntotal = 0;

        sum1 = 0;
        sum2 = 0;

        c_from = pos.peek_piece_at(m.from()).side;
        pt_from = pos.peek_piece_at(m.from()).type;

        N_AMAF = 0;
        sum1_AMAF = 0;
        Mean_AMAF = 0;
    }
};

#endif // NODE_H