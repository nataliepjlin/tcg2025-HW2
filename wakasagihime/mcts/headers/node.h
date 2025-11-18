#ifndef NODE_H
#define NODE_H

#include "../../lib/chess.h"

const int MaxChild = 100;
const int MaxNodes = 100000;

typedef struct MCT_Node{
    Color color;
    int ply; // the ply from parent to here
    int p_id; // parent id, root’s parent is the root
    int c_id[MaxChild]; // children id
    Move moves[MaxChild]; // moves leading to children
    int depth; // depth, 0 for the `root
    int Nchild; // number of children
    int Ntotal; // total # of simulations
    long double CsqrtlogN; // c * sqrt(log(Ntotal))
    long double sqrtN; // sqrt(Ntotal)
    long long sum1; // sum1: sum of scores
    long long sum2; // sum2: sum of square of each score
    long double Mean; // average score
    long double Variance; // variance of score
    bool expandable;

    MCT_Node(){
        color = Black;
        ply = 0;
        depth = 0;
        p_id = -1;
        Nchild = 0;
        Ntotal = 0;
        CsqrtlogN = 0.0L;
        sqrtN = 0.0L;
        sum1 = 0;
        sum2 = 0;
        Mean = 0.0L;
        Variance = 0.0L;
        expandable = true;
    }
    MCT_Node(Color c, int pid){
        color = c;
        ply = 0;
        depth = 0;
        p_id = pid;
        Nchild = 0;
        Ntotal = 0;
        CsqrtlogN = 0.0L;
        sqrtN = 0.0L;
        sum1 = 0;
        sum2 = 0;
        Mean = 0.0L;
        Variance = 0.0L;
        expandable = true;
    }
    MCT_Node(int pid, const MCT_Node &p){
        color = p.color;
        ply = 0;
        depth = p.depth + 1;
        p_id = pid;
        Nchild = 0;
        Ntotal = 0;
        CsqrtlogN = 0.0L;
        sqrtN = 0.0L;
        sum1 = 0;
        sum2 = 0;
        Mean = 0.0L;
        Variance = 0.0L;
        expandable = true;
    }    
}Node;

#endif