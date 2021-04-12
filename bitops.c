#include "bitops.h"

int floor_log2(unsigned long bitboard)
{
    int pos = 0;
    if (bitboard >= (unsigned long)1<<32) { bitboard >>= 32; pos += 32; }
    if (bitboard >= (unsigned long)1<<16) { bitboard >>= 16; pos += 16; }
    if (bitboard >= (unsigned long)1<< 8) { bitboard >>=  8; pos +=  8; }
    if (bitboard >= (unsigned long)1<< 4) { bitboard >>=  4; pos +=  4; }
    if (bitboard >= (unsigned long)1<< 2) { bitboard >>=  2; pos +=  2; }
    if (bitboard >= (unsigned long)1<< 1) {               pos +=  1; }

    return ((bitboard == 0) ? (-1) : pos);
}
//Cuenta el numero de bits en un bitboard
unsigned char count_bits(unsigned long bitboard) {
    unsigned char count = 0;

    while (bitboard) {
        //Restar el bit menos significativo
        bitboard &= bitboard - 1;
        count++;
    }

    // return bit count
    return count;
}
//Devuelve la posicion del primer 1 de la tabla(255 si es un bitboard vacio)
unsigned char get_ls1b_index(unsigned long bitboard)
{
    if (bitboard)
    {
        //Contar bits corridos antes de encontrar un 1
        return count_bits((bitboard & -bitboard) - 1);
    }
    else return 255;
}

// set occupancies
unsigned long set_occupancy(int index, int bits_in_mask, unsigned long attack_mask)
{
    // occupancy map
    unsigned long occupancy = 0ULL;

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