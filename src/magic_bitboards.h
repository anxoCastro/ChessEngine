//
// Created by castro on 12/04/21.
//

#ifndef CHESSENGINEC_MAGIC_BITBOARDS_H
#define CHESSENGINEC_MAGIC_BITBOARDS_H

#include "bitops.h"

bitboard get_rook_attacks(unsigned char square, bitboard occupancy);
bitboard get_bishop_attacks(unsigned char square, bitboard occupancy);
void init_sliders_attacks(int is_bishop);

#endif //CHESSENGINEC_MAGIC_BITBOARDS_H
