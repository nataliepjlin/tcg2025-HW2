#ifndef ZOBRIST_CPP
#define ZOBRIST_CPP

#include "../h/zobrist.h"

uint64_t zob[SIDE_NB][PIECE_TYPE_NB][SQUARE_NB];
pcg64 rng64;

void init_zobrist(){
    rng64.seed(42); // std::random_device{}()
    for(int color = 0; color < SIDE_NB; ++color){
        for(int pieceType = 0; pieceType < PIECE_TYPE_NB; ++pieceType){
            for(int square = 0; square < SQUARE_NB; ++square){
                zob[color][pieceType][square] = rng64();
            }
        }
    }
}

uint64_t compute_zobrist_hash(const Position &pos){
    uint64_t hash = 0;
    for(Square sq: BoardView(pos.pieces())){
        Piece p = pos.peek_piece_at(sq);
        hash ^= zob[p.side][p.type][sq];
    }
    return hash;
}

#endif // ZOBRIST_CPP