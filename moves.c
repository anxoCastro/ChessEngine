//
// Created by anxo on 07/05/20.
//

#include "moves.h"
#include "board.h"
#include "bitops.h"
#include <stdio.h>
#include <stdlib.h>

//Constantes de bitboards equivalentes a partes del tablero utiles para hacer operaciones

const unsigned long not_column_a = 18374403900871474942ULL;
const unsigned long not_column_h = 9187201950435737471ULL;
const unsigned long not_column_hg = 4557430888798830399ULL;
const unsigned long not_column_ab = 18229723555195321596ULL;
const unsigned long row_2 = 71776119061217280UL;
const unsigned long row_7 = 65280UL;
const unsigned long not_row_12 = 281474976710655UL;
const unsigned long not_row_78 = 18446744073709486080UL;
const unsigned long not_row_1 = 72057594037927935UL;
const unsigned long not_row_8 = 18446744073709551360UL;
unsigned long black_pieces;
unsigned long white_pieces;
unsigned long any_pieces;

//Tablas con los movimientos de las piezas
unsigned long knight_move_table[64];
unsigned long king_move_table[64];
unsigned long rook_mask_table[64];
unsigned long bishop_mask_table[64];
//En el caso de los peones cada color tiene sus propias tablas
unsigned long pawn_pushes_table[2][64];
unsigned long pawn_attacks_table[2][64];
unsigned long pawn_promotion_pushes_table[2][64];
unsigned long pawn_promotion_attacks_table[2][64];
//Inicializa una lista de movimientos
moveList *create_move_list(){
    moveList *list = malloc(sizeof(moveList));
    list->nElements = 0;
    return list;
}

//Añade elemento a la lista de elementos
void addElement(moveList *l,unsigned char from, unsigned char  to, unsigned  capture, unsigned long enpassantsquare,
                unsigned enpassant, unsigned  promotion, unsigned  castling){
    l->list[l->nElements].from = from;
    l->list[l->nElements].to = to;
    l->list[l->nElements].enpassantsquare = enpassantsquare;
    l->list[l->nElements].enpassant = enpassant;
    l->list[l->nElements].promotion = promotion;
    l->list[l->nElements].castling = castling;
    l->list[l->nElements].capture = capture;
    l->nElements++;
}


//Generar movimiento de los peones(Vamos a generarlos al inicio de la ejecución del engine)
unsigned long generate_pawn_pushes(int square, unsigned side){
    unsigned long moves = 0UL;

    unsigned long board = 0UL;

    set_bit(board, square);
    if(side == WHITE){
        //Omitimos las dos ultimas filas(coronación)
        if((board) & not_row_78){
            moves |= (board >> 8);
        }
    }
    else{
        //Omitimos las ultimas filas (coronación)
        if((board) & not_row_12){
            moves |= (board << 8);
        }
    }
    return moves;
}

unsigned long generate_pawn_attacks(int square, unsigned side){
    unsigned long moves = 0UL;
    unsigned long board = 0UL;
    set_bit(board, square);
    if(side == WHITE){
        //Omitimos las dos ultimas filas(coronación)
        if((board) & not_row_78){
            moves |= (board>> 7);
            moves |= (board>> 9);
        }
    }
    else{
        //Omitimos las ultimas filas (coronación)
        if((board) & not_row_12){
            moves |= (board<< 7);
            moves |= (board<< 9);
        }
    }
    return moves;
}
unsigned long generate_pawn_promotion_pushes(int square, unsigned side){
    unsigned long moves = 0UL;

    unsigned long board = 0UL;

    set_bit(board, square);

    if(side == WHITE){
        //Solo penultima fila(coronación)
        if((board) & row_7){
            moves |= (board>> 8);
        }
    }
    else{
        //Solo penultima fila(coronación)
        if((board) & row_2){
            moves |= (board<< 8);
        }
    }
    return moves;
}

unsigned long generate_pawn_promotion_attacks(int square, unsigned side){
    unsigned long moves = 0UL;
    unsigned long board = 0UL;
    set_bit(board, square);
    if(side == WHITE){
        //Solo penultima fila(coronación)
        if((board) & row_7){
            moves |= (board>> 7);
            moves |= (board>> 9);
        }
    }
    else{
        //Solo penultima fila(coronación)
        if((board) & row_2){
            moves |= (board<< 7);
            moves |= (board<< 9);
        }
    }
    return moves;
}
//Genera el movimiento de los caballos
unsigned long generate_knight_moves(int square){
    unsigned long moves = 0UL;

    unsigned long board = 0UL;
    set_bit(board, square);
    //Generar movimientos del caballo
    if ((board >> 17) & not_column_h) moves |= (board >> 17);
    if ((board >> 15) & not_column_a) moves |= (board >> 15);
    if ((board >> 10) & not_column_hg) moves |= (board >> 10);
    if ((board >> 6) & not_column_ab) moves |= (board >> 6);
    if ((board << 17) & not_column_a) moves |= (board << 17);
    if ((board << 15) & not_column_h) moves |= (board << 15);
    if ((board << 10) & not_column_ab) moves |= (board << 10);
    if ((board << 6) & not_column_hg) moves |= (board << 6);

    return moves;
}

//Genera el movimiento de los reyes
unsigned long generate_king_moves(int square){
    unsigned long moves = 0UL;
    unsigned long board = 0UL;
    set_bit(board, square);

    //Generar movimientos del rey
    if ((board >> 9) & not_column_h) moves |= (board >> 9);
    if (board >> 8) moves |= (board >> 8);
    if ((board >> 7) & not_column_a) moves |= (board >> 7);
    if ((board >> 1) & not_column_h) moves |= (board >> 1);
    if ((board << 1) & not_column_a) moves |= (board << 1);
    if ((board << 7) & not_column_h) moves |= (board << 7);
    if (board << 8) moves |= (board << 8);
    if ((board << 9) & not_column_a) moves |= (board << 9);

    return moves;
}


//Generar patrones de ataques de torres y alfiles para cada casilla

//Generar mascara movimientos torre
unsigned long generate_rook_mask(int square){
    unsigned long moves = 0UL;
    int i;

    //Calcular fila y columna
    int f = square / 8;
    int c = square % 8;

    // Generar movimientos torre
    for (i = f + 1; i <= 6; i++) moves |= (1ULL << (i * 8 + c));
    for (i = f - 1; i >= 1; i--) moves |= (1ULL << (i * 8 + c));
    for (i = c + 1; i <= 6; i++) moves |= (1ULL << (f * 8 + i));
    for (i = c - 1; i >= 1; i--) moves |= (1ULL << (f * 8 + i));

    return moves;

};

//Generar mascara movimientos alfil
unsigned long generate_bishop_mask(int square){
    unsigned char moves = 0ULL;
    int rk = square/8, fl = square%8, r, f;

    for (r=rk+1, f=fl+1; r<=6 && f<=6; r++, f++) moves |= (1ULL << (f + r*8));
    for (r=rk+1, f=fl-1; r<=6 && f>=1; r++, f--) moves |= (1ULL << (f + r*8));
    for (r=rk-1, f=fl+1; r>=1 && f<=6; r--, f++) moves |= (1ULL << (f + r*8));
    for (r=rk-1, f=fl-1; r>=1 && f>=1; r--, f--) moves |= (1ULL << (f + r*8));

    return moves;


}

//Generar tablas para la generación de los movimientos
void generate_move_tables(){
    //Generamos tabla de ataque de caballos, rey
    for(int i = 0; i < 64; i++){
        knight_move_table[i] = generate_knight_moves(i);
        king_move_table[i] = generate_king_moves(i);
        rook_mask_table[i] = generate_rook_mask(i);
        bishop_mask_table[i] = generate_bishop_mask(i);
        pawn_pushes_table[WHITE][i] = generate_pawn_pushes(i, WHITE);
        pawn_pushes_table[BLACK][i] = generate_pawn_pushes(i, BLACK);
        pawn_attacks_table[WHITE][i] = generate_pawn_attacks(i, WHITE);
        pawn_attacks_table[BLACK][i] = generate_pawn_attacks(i, BLACK);
        pawn_promotion_pushes_table[WHITE][i] = generate_pawn_promotion_pushes(i, WHITE);
        pawn_promotion_pushes_table[BLACK][i] = generate_pawn_promotion_pushes(i, BLACK);
        pawn_promotion_attacks_table[WHITE][i] = generate_pawn_promotion_attacks(i, WHITE);
        pawn_promotion_attacks_table[BLACK][i] = generate_pawn_promotion_attacks(i, BLACK);
    }
}


//Calcular ataque del alfil
unsigned long bishop_moves(int square, unsigned long all_pieces)
{
    unsigned long moves = 0UL;

    int f, r;

    //Calcular fila y columna
    int tr = square / 8;
    int tf = square % 8;

    // generate attacks
    for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++)
    {
        moves |= (1ULL << (r * 8 + f));
        if (all_pieces & (1ULL << (r * 8 + f))) break;
    }

    for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--)
    {
        moves |= (1ULL << (r * 8 + f));
        if (all_pieces & (1ULL << (r * 8 + f))) break;
    }

    for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++)
    {
        moves |= (1ULL << (r * 8 + f));
        if (all_pieces & (1ULL << (r * 8 + f))) break;
    }

    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--)
    {
        moves |= (1ULL << (r * 8 + f));
        if (all_pieces & (1ULL << (r * 8 + f))) break;
    }

    return moves;
}

//Calcular ataque de la torre
unsigned long rook_moves(int square, unsigned long all_pieces)
{
    unsigned long moves = 0UL;
    int f, r;

    //Calcular fila y columna
    int tr = square / 8;
    int tf = square % 8;

    // generate attacks
    for (r = tr + 1; r <= 7; r++)
    {
        moves |= (1ULL << (r * 8 + tf));
        if (all_pieces & (1ULL << (r * 8 + tf))) break;
    }

    for (r = tr - 1; r >= 0; r--)
    {
        moves |= (1ULL << (r * 8 + tf));
        if (all_pieces & (1ULL << (r * 8 + tf))) break;
    }

    for (f = tf + 1; f <= 7; f++)
    {
        moves |= (1ULL << (tr * 8 + f));
        if (all_pieces & (1ULL << (tr * 8 + f))) break;
    }

    for (f = tf - 1; f >= 0; f--)
    {
        moves |= (1ULL << (tr * 8 + f));
        if (all_pieces & (1ULL << (tr * 8 + f))) break;
    }

    return moves;
}

//Pendiente de implementar
moveList *generate_legal_moves(board *b, move lastMove){
    if(b->side == BLACK){
        generate_black_moves(b, lastMove);
    }
    else{
        generate_white_moves(b, lastMove);
    }
}

moveList *generate_black_moves(board *b, move lastMove){
    //Creamos la lista
    moveList *mL = create_move_list();

    //Posición inicial y final de la pieza movida
    unsigned char from;
    unsigned char to;
    //Bitboards que representa todas las piezas negras blancas y todas las negras
    black_pieces = b->BP | b->BN | b->BB | b->BR | b->BQ | b->BK;
    white_pieces = b->WP | b->WN | b->WB | b->WR | b->WQ | b->WK;
    any_pieces = black_pieces | white_pieces;

    unsigned long aux;
    unsigned long pawn_attacks;
    unsigned long enpassant_attacks;
    unsigned long promotion_pushes;
    unsigned long promotion_attacks;
    //Peones negros
    unsigned long BP = b->BP;
    while(BP){
        from = get_ls1b_index(BP);
        //Avance peones
        aux = pawn_pushes_table[BLACK][from] & ~any_pieces;
        //Avance una casilla(omitimos penultima fila)
        if(aux) {
            to = get_ls1b_index(aux);
            addElement(mL, from, to, 0, 0, 0, 0, 0);
        }
        //Avance dos casillas
        if((from < 16) && ((~any_pieces>>from+8)&1) && ((~any_pieces>>from+16)&1)){
            to = from + 16;
            addElement(mL, from, to, 0, 1UL<<to, 1, 0, 0);
        }
        //Captura peones
        pawn_attacks = pawn_attacks_table[BLACK][from] & white_pieces;
        while(pawn_attacks){
            to = get_ls1b_index(pawn_attacks);
            addElement(mL, from, to, 1, 0, 0, 0, 0);
            pop_bit(pawn_attacks, to);
        }

        //Captura al paso
        if(lastMove.enpassant){
            enpassant_attacks = pawn_attacks_table[BLACK][from-8] & lastMove.enpassantsquare;
            if(enpassant_attacks){
                to = get_ls1b_index(enpassant_attacks);
                addElement(mL, from, to, 1, 0, 0, 0, 0);
            }
        }
        //Promocion peon
        promotion_pushes = pawn_promotion_pushes_table[BLACK][from] & ~any_pieces;
        if(promotion_pushes){
            to = get_ls1b_index(promotion_pushes);
            addElement(mL, from, to, 0, 0, 0, QUEEN, 0);
            addElement(mL, from, to, 0, 0, 0, ROOK, 0);
            addElement(mL, from, to, 0, 0, 0, KNIGHT, 0);
            addElement(mL, from, to, 0, 0, 0, BISHOP, 0);
        }
        //Promocion capturando
        promotion_attacks = pawn_promotion_attacks_table[BLACK][from] & white_pieces;
        while (promotion_attacks){
            to = get_ls1b_index(promotion_attacks);
            addElement(mL, from, to, 1, 0, 0, QUEEN, 0);
            addElement(mL, from, to, 1, 0, 0, ROOK, 0);
            addElement(mL, from, to, 1, 0, 0, KNIGHT, 0);
            addElement(mL, from, to, 1, 0, 0, BISHOP, 0);
            pop_bit(promotion_attacks, to);
        }
        pop_bit(BP, from);
    }

    //Caballos negros
    unsigned long BN = b->BN;
    while(BN){
        //Bitboard con los posibles movimientos de los caballos
        from = get_ls1b_index(BN);
        aux = knight_move_table[from]  & ~black_pieces;
        while(aux) {
            to = get_ls1b_index(aux);
            //Si captura pieza
            if(to && get_bit(white_pieces, to)) addElement(mL, from, to, 1, 0, 0, 0, 0);
            else addElement(mL, from, to, 0, 0, 0, 0, 0);
            pop_bit(aux, to);
        }
        pop_bit(BN, from);

    }

    //Rey negro
    from = get_ls1b_index(b->BK);
    //Consultamos los movimientos y descartamos aquellos que no se pueden hacer por haber una pieza negra
    aux = king_move_table[from]  & ~black_pieces;
    while(aux) {
        to = get_ls1b_index(aux);
        //Si captura pieza
        if(to && get_bit(white_pieces, to)) addElement(mL, from, to, 1, 0, 0, 0, 0);
        else addElement(mL, from, to, 0, 0, 0, 0, 0);
        pop_bit(aux, to);
    }

    /*Aquí usaríamos magic bitboards para calcular los movimientos pero por ahora voy a dejarlo así*/
    /*Por ahora se calcula en tiempo de ejecución*/
    //Torres negras
    unsigned long BR = b->BR;
    while(BR){
        from = get_ls1b_index(BR);
        //Calculamos los posibles movimientos de la torre
        aux = rook_moves(from, any_pieces) & ~black_pieces;
        while(aux){
            to = get_ls1b_index(aux);
            //Si captura pieza
            if(to && get_bit(white_pieces, to)) addElement(mL, from, to, 1, 0, 0, 0, 0);
            else addElement(mL, from, to, 0, 0, 0, 0, 0);
            pop_bit(aux, to);
        }
        pop_bit(BR, from);
    }

    //Alfiles negros
    unsigned long BB = b->BB;
    while(BB){
        from = get_ls1b_index(BB);
        //Calculamos los posibles movimientos de la torre
        aux = bishop_moves(from, any_pieces) & ~black_pieces;
        while(aux){
            to = get_ls1b_index(aux);
            //Si captura pieza
            if(to && get_bit(white_pieces, to)) addElement(mL, from, to, 1, 0, 0, 0, 0);
            else addElement(mL, from, to, 0, 0, 0, 0, 0);
            pop_bit(aux, to);
        }
        pop_bit(BB, from);
    }

    //Reina negra
    if(b->BQ){
        from = get_ls1b_index(b->BQ);
        //Calculamos los movimientos de la reina
        aux = (bishop_moves(from, any_pieces) | rook_moves(from, any_pieces)) & ~black_pieces;
        while(aux){
            to = get_ls1b_index(aux);
            //Si captura pieza
            if(to && get_bit(white_pieces, to)) addElement(mL, from, to, 1, 0, 0, 0, 0);
            else addElement(mL, from, to, 0, 0, 0, 0, 0);
            pop_bit(aux, to);
        }
    }
    return mL;

}


moveList *generate_white_moves(board *b, move lastMove){
    //Creamos la lista
    moveList *mL = create_move_list();

    //Posición inicial y final de la pieza movida
    unsigned char from;
    unsigned char to;
    //Bitboards que representa todas las piezas negras blancas y todas las negras
    black_pieces = b->BP | b->BN | b->BB | b->BR | b->BQ | b->BK;
    white_pieces = b->WP | b->WN | b->WB | b->WR | b->WQ | b->WK;
    any_pieces = black_pieces | white_pieces;

    unsigned long aux;
    unsigned long pawn_attacks;
    unsigned long enpassant_attacks;
    unsigned long promotion_pushes;
    unsigned long promotion_attacks;
    //Peones blancos
    unsigned long WP = b->WP;
    while(WP){
        from = get_ls1b_index(WP);
        //Avance peones
        aux = pawn_pushes_table[WHITE][from] & ~any_pieces;
        //Avance una casilla(omitimos penultima fila)
        if(aux) {
            to = get_ls1b_index(aux);
            addElement(mL, from, to, 0, 0, 0, 0, 0);
        }
        //Avance dos casillas
        if((from > 47) && ((~any_pieces>>from-8)&1) && ((~any_pieces>>from-16)&1)){
            to = from - 16;
            addElement(mL, from, to, 0, 1UL<<to, 1, 0, 0);
        }
        //Captura peones
        pawn_attacks = pawn_attacks_table[WHITE][from] & black_pieces;
        while(pawn_attacks){
            to = get_ls1b_index(pawn_attacks);
            addElement(mL, from, to, 1, 0, 0, 0, 0);
            pop_bit(pawn_attacks, to);
        }

        //Captura al paso
        if(lastMove.enpassant){
            enpassant_attacks = pawn_attacks_table[WHITE][from+8] & lastMove.enpassantsquare;
            if(enpassant_attacks){
                to = get_ls1b_index(enpassant_attacks);
                addElement(mL, from, to, 1, 0, 0, 0, 0);
            }
        }
        //Promocion peon
        promotion_pushes = pawn_promotion_pushes_table[WHITE][from] & ~any_pieces;
        if(promotion_pushes){
            to = get_ls1b_index(promotion_pushes);
            addElement(mL, from, to, 0, 0, 0, QUEEN, 0);
            addElement(mL, from, to, 0, 0, 0, ROOK, 0);
            addElement(mL, from, to, 0, 0, 0, KNIGHT, 0);
            addElement(mL, from, to, 0, 0, 0, BISHOP, 0);
        }

        //Promocion capturando
        promotion_attacks = pawn_promotion_attacks_table[WHITE][from] & black_pieces;
        while (promotion_attacks){
            to = get_ls1b_index(promotion_attacks);
            addElement(mL, from, to, 1, 0, 0, QUEEN, 0);
            addElement(mL, from, to, 1, 0, 0, ROOK, 0);
            addElement(mL, from, to, 1, 0, 0, KNIGHT, 0);
            addElement(mL, from, to, 1, 0, 0, BISHOP, 0);
            pop_bit(promotion_attacks, to);
        }
        pop_bit(WP, from);
    }

    //Caballos blancos
    unsigned long WN = b->WN;
    while(WN){
        //Bitboard con los posibles movimientos de los caballos
        from = get_ls1b_index(WN);
        aux = knight_move_table[from]  & ~white_pieces;
        while(aux) {
            to = get_ls1b_index(aux);
            //Si captura pieza
            if(to && get_bit(black_pieces, to)) addElement(mL, from, to, 1, 0, 0, 0, 0);
            else addElement(mL, from, to, 0, 0, 0, 0, 0);
            pop_bit(aux, to);
        }
        pop_bit(WN, from);

    }

    //Rey blancos
    from = get_ls1b_index(b->WK);
    //Consultamos los movimientos y descartamos aquellos que no se pueden hacer por haber una pieza blanca
    aux = king_move_table[from]  & ~white_pieces;
    while(aux) {
        to = get_ls1b_index(aux);
        //Si captura pieza
        if(to && get_bit(black_pieces, to)) addElement(mL, from, to, 1, 0, 0, 0, 0);
        else addElement(mL, from, to, 0, 0, 0, 0, 0);
        pop_bit(aux, to);
    }

    /*Aquí usaríamos magic bitboards para calcular los movimientos pero por ahora voy a dejarlo así*/
    /*Por ahora se calcula en tiempo de ejecución*/
    //Torres blancas
    unsigned long WR = b->WR;
    while(WR){
        from = get_ls1b_index(WR);
        //Calculamos los posibles movimientos de la torre
        aux = rook_moves(from, any_pieces) & ~white_pieces;
        while(aux){
            to = get_ls1b_index(aux);
            //Si captura pieza
            if(to && get_bit(black_pieces, to)) addElement(mL, from, to, 1, 0, 0, 0, 0);
            else addElement(mL, from, to, 0, 0, 0, 0, 0);
            pop_bit(aux, to);
        }
        pop_bit(WR, from);
    }

    //Alfiles blancos
    unsigned long WB = b->WB;
    while(WB){
        from = get_ls1b_index(WB);
        //Calculamos los posibles movimientos de la torre
        aux = bishop_moves(from, any_pieces) & ~white_pieces;
        while(aux){
            to = get_ls1b_index(aux);
            //Si captura pieza
            if(to && get_bit(black_pieces, to)) addElement(mL, from, to, 1, 0, 0, 0, 0);
            else addElement(mL, from, to, 0, 0, 0, 0, 0);
            pop_bit(aux, to);
        }
        pop_bit(WB, from);
    }

    //Reina blanca
    if(b->WQ){
        from = get_ls1b_index(b->WQ);
        //Calculamos los movimientos de la reina
        aux = (bishop_moves(from, any_pieces) | rook_moves(from, any_pieces)) & ~white_pieces;
        while(aux){
            to = get_ls1b_index(aux);
            //Si captura pieza
            if(to && get_bit(black_pieces, to)) addElement(mL, from, to, 1, 0, 0, 0, 0);
            else addElement(mL, from, to, 0, 0, 0, 0, 0);
            pop_bit(aux, to);
        }
    }
    return mL;

}
//Comprobar si el rey está en jaque, 1 si está 0 si no lo está
int is_in_check(board *b){
    //Bitboards que representa todas las piezas negras blancas y todas las negras
    black_pieces = b->BP | b->BN | b->BB | b->BR | b->BQ | b->BK;
    white_pieces = b->WP | b->WN | b->WB | b->WR | b->WQ | b->WK;
    any_pieces = black_pieces | white_pieces;

    //Juegan negras
    if(b->side){
        //Casilla del rey
        int square = get_ls1b_index(b->BK);
        //Comprobar si algún caballo está atacando al rey
        if(b->WK & knight_move_table[square]) return -1;
        //Alfiles
        if(b->WB & bishop_moves(square, any_pieces) & ~white_pieces) return -1;

    }
    //Si juegan blancas
    else{
        //Comprobar si algún caballo está atacando al rey
        //Casilla del rey
        int square = get_ls1b_index(b->WK);
        if(b->BK & knight_move_table[square]) return -1;
    }

}

void print_bitboard(unsigned long b) {
    //Convertir bitboard a caracteres
    char charBoard[8][8] = {[0 ... 7][0 ... 7] = '-'};
    for (int i = 0; i < 64; i++) {
        if(((b>>i)&1)==1) {
            charBoard[i / 8][i % 8] = '+';
        }

    }
    //Imprimir tablero
    for (int i = 0; i < 8; i++) {
        //Una fila del tablero
        printf("%c%c%c%c%c%c%c%c\n", charBoard[i][0], charBoard[i][1], charBoard[i][2],
               charBoard[i][3], charBoard[i][4], charBoard[i][5], charBoard[i][6], charBoard[i][7]);
    }
}

//Convertir movimiento(notación en forma de origen + destino)
//Falta promocion
void move_to_string(move *m, char *string){
    //Origen
    string[0] = 'a' + (m->from % 8);
    string[1]  = '1' + 7 - (m->from / 8);
    //Destino
    string[2] = 'a' + (m->to % 8);
    string[3] = '1' + 7 - (m->to / 8);
}

void string_to_move(char *string, move *m){
    //Origen
    m->from = ((string[1] - '1') * 8) + (string[0] - 'a');
    //Destino
    m->to = ((string[3] - '1') * 8) + (string[2] - 'a');
}