#ifndef CHESSENGINEC_BITOPS_H
#define CHESSENGINEC_BITOPS_H

typedef unsigned long long bitboard;

int floor_log2(bitboard b);
unsigned char count_bits(bitboard b);
unsigned char get_ls1b_index(bitboard b);
bitboard set_occupancy(int index, int bits_in_mask, bitboard attack_mask);
//Operaciones de bit sobre el bitboard
#define set_bit(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define get_bit(bitboard, square) ((bitboard) & (1ULL << (square)))
#define pop_bit(bitboard, square) (get_bit(bitboard, square) ? (bitboard) ^= (1ULL << (square)) : 0)

#endif //CHESSENGINEC_BITOPS_H
