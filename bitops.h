#ifndef CHESSENGINEC_BITOPS_H
#define CHESSENGINEC_BITOPS_H

int floor_log2(unsigned long bitboard);
unsigned char count_bits(unsigned long bitboard);
unsigned char get_ls1b_index(unsigned long bitboard);
unsigned long set_occupancy(int index, int bits_in_mask, unsigned long attack_mask);
//Operaciones de bit sobre el bitboard
#define set_bit(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define get_bit(bitboard, square) ((bitboard) & (1ULL << (square)))
#define pop_bit(bitboard, square) (get_bit(bitboard, square) ? (bitboard) ^= (1ULL << (square)) : 0)

#endif //CHESSENGINEC_BITOPS_H
