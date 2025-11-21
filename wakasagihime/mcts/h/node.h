#ifndef NODE_H
#define NODE_H

#include <vector>
#include "../../lib/chess.h"
const int MaxChild = 64;

class MCTSNode{
public:
    Move ply;  // the ply from parent to here
    int p_id; // parent id, root’s parent is the root
    int c_id[MaxChild]; // children id
    int depth; // depth, 0 for the root
    int Nchild; // number of children
    int Ntotal; // total # of simulations
    long double CsqrtlogN; // c * sqrt(log(Ntotal))
    long double sqrtN; // sqrt(Ntotal)
    long long sum1; // sum1: sum of scores
    long long sum2; // sum2: sum of square of each score
    long double Mean; // average score, i.e. win rate
    long double Variance; // variance of score

    int AMAF_N;          // Number of times this move appeared in ANY descendant playout
    long double AMAF_sum;// Sum of scores for those playouts
    long double AMAF_Mean; // AMAF win rate

    MCTSNode(int pid = 0, int d = 0){
        p_id = pid;
        depth = d;
        Nchild = 0;
        Ntotal = 0;

        sum1 = 0;
        sum2 = 0;
        Mean = 0.0L;
        Variance = 0.0L;

        AMAF_N = 0;
        AMAF_sum = 0.0L;
        AMAF_Mean = 0.0L;
    }

    MCTSNode(int pid, int d, Move m){
        p_id = pid;
        depth = d;
        ply = m;
        Nchild = 0;
        Ntotal = 0;

        sum1 = 0;
        sum2 = 0;
        Mean = 0.0L;
        Variance = 0.0L;

        AMAF_N = 0;
        AMAF_sum = 0.0L;
        AMAF_Mean = 0.0L;
    }
};

#endif // NODE_H