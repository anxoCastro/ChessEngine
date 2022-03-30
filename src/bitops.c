#include "bitops.h"

int floor_log2(bitboard b)
{
    int pos = 0;
    if (b >= (bitboard)1<<32) { b >>= 32; pos += 32; }
    if (b >= (bitboard)1<<16) { b >>= 16; pos += 16; }
    if (b >= (bitboard)1<< 8) { b >>=  8; pos +=  8; }
    if (b >= (bitboard)1<< 4) { b >>=  4; pos +=  4; }
    if (b >= (bitboard)1<< 2) { b >>=  2; pos +=  2; }
    if (b >= (bitboard)1<< 1) {               pos +=  1; }

    return ((b == 0) ? (-1) : pos);
}
//Cuenta el numero de bits en un bitboard
unsigned char count_bits(bitboard b) {

    unsigned int count = 0;
    while (b) {
        //Restar el bit menos significativo
        b &= b - 1;
        count++;
    }

    // return bit count
    return count;
}
//Devuelve la posicion del primer 1 de la tabla(255 si es un bitboard vacio)
unsigned char get_ls1b_index(bitboard b)
{
    /*if (b)
    {
        //Contar bits corridos antes de encontrar un 1
        return count_bits((b & -b) - 1);
    }
    else{
        return 255;
    }*/

    unsigned char ls1b = __builtin_ffsll(b);
    if(ls1b)
        return ls1b - 1;
    else return 255;
}

// set occupancies
bitboard set_occupancy(int index, int bits_in_mask, bitboard attack_mask)
{
    // occupancy map
    bitboard occupancy = 0ULL;

    // loop over the range of bits within attack mask
    for (int count = 0; count < bits_in_mask; count++)
    {
        // get LS1B index of attacks mask
        int square = get_ls1b_index(attack_mask);

        // pop LS1B in attack map
        pop_bit(attack_mask, square);

        // make sure occupancy is on board
        if (index & (1 << count))
            // populate occupancy map
            occupancy |= (1ULL << square);
    }

    // return occupancy map
    return occupancy;
}