//
// Created by castro on 12/04/21.
//

#ifndef CHESSENGINEC_MAGIC_BITBOARDS_H
#define CHESSENGINEC_MAGIC_BITBOARDS_H
unsigned long get_rook_attacks(unsigned char square, unsigned long occupancy);
unsigned long get_bishop_attacks(unsigned char square, unsigned long occupancy);
void init_sliders_attacks(int is_bishop);

#endif //CHESSENGINEC_MAGIC_BITBOARDS_H
