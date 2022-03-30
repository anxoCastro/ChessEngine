//
// Created by castro on 20/05/21.
//

#include "bitops.h"

#ifndef CHESSENGINEC_TRANSPOSITION_H
#define CHESSENGINEC_TRANSPOSITION_H

#endif //CHESSENGINEC_TRANSPOSITION_H

typedef struct{
    bitboard hash_key;
    unsigned long depth;
    unsigned long long nodes;

}transposition_table_item;