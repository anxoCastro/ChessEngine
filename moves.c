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
                unsigned enpassant, unsigned  promotion, unsigned  castling, unsigned piece){
    l->list[l->nElements].from = from;
    l->list[l->nElements].to = to;
    l->list[l->nElements].enpassantsquare = enpassantsquare;
    l->list[l->nElements].enpassant = enpassant;
    l->list[l->nElements].promotion = promotion;
    l->list[l->nElements].castling = castling;
    l->list[l->nElements].capture = capture;
    l->list[l->nElements].piece = piece;
    l->nElements++;
}

void initMove(move *m){
    m->from = 0;
    m->to = 0;
    m->piece = 6;
    m->enpassantsquare = 0;
    m->enpassant = 0;
    m->promotion = 0;
    m->castling = 0;
    m->capture = 0;
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
            if(board & not_column_h)moves |= (board>> 7);
            if(board & not_column_a)moves |= (board>> 9);
        }
    }
    else{
        //Omitimos las ultimas filas (coronación)
        if((board) & not_row_12){
            if(board & not_column_a)moves |= (board<< 7);
            if(board & not_column_h)moves |= (board<< 9);
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
            if(board & not_column_h)moves |= (board>> 7);
            if(board & not_column_a)moves |= (board>> 9);
        }
    }
    else{
        //Solo penultima fila(coronación)
        if((board) & row_2){
            if(board & not_column_a)moves |= (board<< 7);
            if(board & not_column_h)moves |= (board<< 9);
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
        //generate_black_moves(b, lastMove, );
    }
    else{
        //generate_white_moves(b, lastMove);
    }
}

moveList *generate_black_moves(board *b, move lastMove, moveList *mL){
    //Creamos la lista
    //moveList *mL = create_move_list();

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
            addElement(mL, from, to, 0, 0, 0, 0, 0, PAWN);
        }
        //Avance dos casillas
        if((from < 16) && ((~any_pieces>>from+8)&1) && ((~any_pieces>>from+16)&1)){
            to = from + 16;
            addElement(mL, from, to, 0, 1UL<<to, 0, 0, 0, PAWN);
        }

        //Captura peones
        pawn_attacks = pawn_attacks_table[BLACK][from] & white_pieces;
        while(pawn_attacks){
            to = get_ls1b_index(pawn_attacks);
            addElement(mL, from, to, 1, 0, 0, 0, 0, PAWN);
            pop_bit(pawn_attacks, to);
        }

        //Captura al paso
        if(lastMove.enpassantsquare){
            enpassant_attacks = pawn_attacks_table[BLACK][from-8] & lastMove.enpassantsquare;
            if(enpassant_attacks){
                to = get_ls1b_index(enpassant_attacks);
                addElement(mL, from, to, 1, 0, 1, 0, 0, PAWN);
            }
        }
        //Promocion avanzando
        promotion_pushes = pawn_promotion_pushes_table[BLACK][from] & ~any_pieces;
        if(promotion_pushes){
            to = get_ls1b_index(promotion_pushes);
            addElement(mL, from, to, 0, 0, 0, QUEEN, 0, PAWN);
            addElement(mL, from, to, 0, 0, 0, ROOK, 0, PAWN);
            addElement(mL, from, to, 0, 0, 0, KNIGHT, 0, PAWN);
            addElement(mL, from, to, 0, 0, 0, BISHOP, 0, PAWN);
        }
        //Promocion capturando
        promotion_attacks = pawn_promotion_attacks_table[BLACK][from] & white_pieces;
        while (promotion_attacks){
            to = get_ls1b_index(promotion_attacks);
            addElement(mL, from, to, 1, 0, 0, QUEEN, 0, PAWN);
            addElement(mL, from, to, 1, 0, 0, ROOK, 0, PAWN);
            addElement(mL, from, to, 1, 0, 0, KNIGHT, 0, PAWN);
            addElement(mL, from, to, 1, 0, 0, BISHOP, 0, PAWN);
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
            if(to && get_bit(white_pieces, to)) addElement(mL, from, to, 1, 0, 0, 0, 0, KNIGHT);
            else addElement(mL, from, to, 0, 0, 0, 0, 0, KNIGHT);
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
        if(to && get_bit(white_pieces, to)) addElement(mL, from, to, 1, 0, 0, 0, 0, KING);
        else addElement(mL, from, to, 0, 0, 0, 0, 0, KING);
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
            if(to && get_bit(white_pieces, to)) addElement(mL, from, to, 1, 0, 0, 0, 0, ROOK);
            else addElement(mL, from, to, 0, 0, 0, 0, 0, ROOK);
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
            if(to && get_bit(white_pieces, to)) addElement(mL, from, to, 1, 0, 0, 0, 0, BISHOP);
            else addElement(mL, from, to, 0, 0, 0, 0, 0, BISHOP);
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
            if(to && get_bit(white_pieces, to)) addElement(mL, from, to, 1, 0, 0, 0, 0, QUEEN);
            else addElement(mL, from, to, 0, 0, 0, 0, 0, QUEEN);
            pop_bit(aux, to);
        }
    }

    //Enroque negro corto
    if(b->castle[2]){
        //Si no hay ninguna pieza entre rey y torre y no están siendo atacadas las casillas
        if(!get_bit(any_pieces, 5) && !get_bit(any_pieces, 6)){
            if(!is_attacked(b, 4, BLACK) && !is_attacked(b, 5, BLACK) && !is_attacked(b, 6, BLACK)  && !is_attacked(b, 7, BLACK) ) {
                b->castle[2] = 0;
                b->castle[3] = 0;
                addElement(mL, 4, 6, 0, 0, 0, 0, 1, KING);
            }
        }
    }
    //Enroque largo negro
    if(b->castle[3]) {
        if (!get_bit(any_pieces, 1) && !get_bit(any_pieces, 2) && !get_bit(any_pieces, 3)) {
            if (!is_attacked(b, 0, BLACK) && !is_attacked(b, 1, BLACK) && !is_attacked(b, 2, BLACK) &&
                !is_attacked(b, 3, BLACK) && !is_attacked(b, 4, BLACK)) {
                b->castle[2] = 0;
                b->castle[3] = 0;
                addElement(mL, 4, 2, 0, 0, 0, 0, 1, KING);
            }
        }
    }
    return mL;

}


moveList *generate_white_moves(board *b, move lastMove, moveList *mL){
    //Creamos la lista
    //moveList *mL = create_move_list();

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
            addElement(mL, from, to, 0, 0, 0, 0, 0, PAWN);
        }
        //Avance dos casillas
        if((from > 47) && ((~any_pieces>>from-8)&1) && ((~any_pieces>>from-16)&1)){
            to = from - 16;
            addElement(mL, from, to, 0, 1UL<<to, 0, 0, 0, PAWN);
        }
        //Captura peones

        pawn_attacks = pawn_attacks_table[WHITE][from] & black_pieces;
        while(pawn_attacks){
            to = get_ls1b_index(pawn_attacks);
            addElement(mL, from, to, 1, 0, 0, 0, 0, PAWN);
            pop_bit(pawn_attacks, to);
        }

        //Captura al paso

        if(lastMove.enpassantsquare){
            enpassant_attacks = pawn_attacks_table[WHITE][from+8] & lastMove.enpassantsquare;
            if(enpassant_attacks){
                to = get_ls1b_index(enpassant_attacks);
                addElement(mL, from, to, 1, 0, 1, 0, 0, PAWN);
            }
        }
        //Promocion avanzando
        promotion_pushes = pawn_promotion_pushes_table[WHITE][from] & ~any_pieces;
        if(promotion_pushes){
            to = get_ls1b_index(promotion_pushes);
            addElement(mL, from, to, 0, 0, 0, QUEEN, 0, PAWN);
            addElement(mL, from, to, 0, 0, 0, ROOK, 0, PAWN);
            addElement(mL, from, to, 0, 0, 0, KNIGHT, 0, PAWN);
            addElement(mL, from, to, 0, 0, 0, BISHOP, 0, PAWN);
        }

        //Promocion capturando
        promotion_attacks = pawn_promotion_attacks_table[WHITE][from] & black_pieces;
        while (promotion_attacks){
            to = get_ls1b_index(promotion_attacks);
            addElement(mL, from, to, 1, 0, 0, QUEEN, 0, PAWN);
            addElement(mL, from, to, 1, 0, 0, ROOK, 0, PAWN);
            addElement(mL, from, to, 1, 0, 0, KNIGHT, 0, PAWN);
            addElement(mL, from, to, 1, 0, 0, BISHOP, 0, PAWN);
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
            if(to && get_bit(black_pieces, to)) addElement(mL, from, to, 1, 0, 0, 0, 0, KNIGHT);
            else addElement(mL, from, to, 0, 0, 0, 0, 0, KNIGHT);
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
        if(to && get_bit(black_pieces, to)) addElement(mL, from, to, 1, 0, 0, 0, 0, KING);
        else addElement(mL, from, to, 0, 0, 0, 0, 0, KING);
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
            if(to && get_bit(black_pieces, to)) addElement(mL, from, to, 1, 0, 0, 0, 0, ROOK);
            else addElement(mL, from, to, 0, 0, 0, 0, 0, ROOK);
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
            if(to && get_bit(black_pieces, to)) addElement(mL, from, to, 1, 0, 0, 0, 0, BISHOP);
            else addElement(mL, from, to, 0, 0, 0, 0, 0, BISHOP);
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
            if(to && get_bit(black_pieces, to)) addElement(mL, from, to, 1, 0, 0, 0, 0, QUEEN);
            else addElement(mL, from, to, 0, 0, 0, 0, 0, QUEEN);
            pop_bit(aux, to);
        }
    }
    //Enroque blanco corto
    if(b->castle[0]){
        //Si no hay ninguna pieza entre rey y torre y no están siendo atacadas las casillas
        if(!get_bit(any_pieces, 61) && !get_bit(any_pieces, 62)){
            if(!is_attacked(b, 60, BLACK) && !is_attacked(b, 61, BLACK) && !is_attacked(b, 62, BLACK)  && !is_attacked(b, 63, BLACK) ) {
                b->castle[0] = 0;
                b->castle[1] = 0;
                addElement(mL, 60, 62, 0, 0, 0, 0, 1, KING);
            }
        }
    }
    //Enroque largo blanco
    if(b->castle[1]){
        if(!get_bit(any_pieces, 57) && !get_bit(any_pieces, 58) && !get_bit(any_pieces, 59)) {
            if (!is_attacked(b, 56, BLACK) && !is_attacked(b, 57, BLACK) && !is_attacked(b, 58, BLACK) &&
                !is_attacked(b, 59, BLACK) && !is_attacked(b, 60, BLACK)) {
                b->castle[1] = 0;
                b->castle[0] = 0;
                addElement(mL, 60, 58, 0, 0, 0, 0, 1, KING);
            }
        }
    }
    return mL;

}

//Realizar movimiento(no comprobamos si es legal o no en este momento)
void make_move(board *b, move m, unmake_stack *unmakeStack){
    unmake_info unmakeInfo;
    unmakeInfo.castle[0] = 0;
    unmakeInfo.castle[1] = 0;
    unmakeInfo.castle[2] = 0;
    unmakeInfo.castle[3] = 0;
    unmakeInfo.capture_piece = 6;
    unmakeInfo.capture_enpassant = 6;
    switch (m.piece) {
        case PAWN:
            if(b->side == WHITE){
                pop_bit(b->WP, m.from);
                set_bit(b->WP, m.to);
                //Si es una promoción transformar la pieza
                if(m.promotion){
                    pop_bit(b->WP, m.to);
                    switch (m.promotion) {
                        case KNIGHT:
                            set_bit(b->WN, m.to);
                            break;
                        case BISHOP:
                            set_bit(b->WB, m.to);
                            break;
                        case ROOK:
                            set_bit(b->WR, m.to);
                            break;
                        case QUEEN:
                            set_bit(b->WQ, m.to);
                            break;
                    }
                }
                //Si hay captura al paso borramos la pieza capturada
                if(m.enpassant){
                    if(get_bit(b->BP, m.to + 8)){
                        pop_bit(b->BP, m.to + 8);
                        unmakeInfo.capture_enpassant = PAWN;
                    }
                    else if(get_bit(b->BN, m.to + 8)){
                        pop_bit(b->BN, m.to + 8);
                        unmakeInfo.capture_enpassant = KNIGHT;
                    }
                    else if(get_bit(b->BB, m.to + 8)){
                        pop_bit(b->BB, m.to + 8);
                        unmakeInfo.capture_enpassant = BISHOP;
                    }
                    else if(get_bit(b->BR, m.to + 8)){
                        pop_bit(b->BR, m.to + 8);
                        unmakeInfo.capture_enpassant = ROOK;
                    }
                    else if(get_bit(b->BQ, m.to + 8)){
                        pop_bit(b->BQ, m.to + 8);
                        unmakeInfo.capture_enpassant = QUEEN;
                    }
                }
            }
            else{
                pop_bit(b->BP, m.from);
                set_bit(b->BP, m.to);
                //Si es una promoción transformar la pieza
                if(m.promotion){
                    pop_bit(b->WP, m.to);
                    switch (m.promotion) {
                        case KNIGHT:
                            set_bit(b->BN, m.to);
                            break;
                        case BISHOP:
                            set_bit(b->BB, m.to);
                            break;
                        case ROOK:
                            set_bit(b->BR, m.to);
                            break;
                        case QUEEN:
                            set_bit(b->BQ, m.to);
                            break;
                    }
                }
                //Si hay captura al paso borramos la pieza capturada
                if(m.enpassant){
                    if(get_bit(b->WP, m.to - 8)){
                        pop_bit(b->WP, m.to - 8);
                        unmakeInfo.capture_enpassant = PAWN;
                    }
                    else if(get_bit(b->WN, m.to - 8)){
                        pop_bit(b->WN, m.to - 8);
                        unmakeInfo.capture_enpassant = KNIGHT;
                    }
                    else if(get_bit(b->WB, m.to - 8)){
                        pop_bit(b->WB, m.to - 8);
                        unmakeInfo.capture_enpassant = BISHOP;
                    }
                    else if(get_bit(b->WR, m.to - 8)){
                        pop_bit(b->WR, m.to - 8);
                        unmakeInfo.capture_enpassant = ROOK;
                    }
                    else if(get_bit(b->WQ, m.to - 8)){
                        pop_bit(b->WQ, m.to - 8);
                        unmakeInfo.capture_enpassant = QUEEN;
                    }
                }
            }
            break;
        case KNIGHT:
            if(b->side == WHITE){
                pop_bit(b->WN, m.from);
                set_bit(b->WN, m.to);
            }
            else{
                pop_bit(b->BN, m.from);
                set_bit(b->BN, m.to);
            }
            break;
        case BISHOP:
            if(b->side == WHITE){
                pop_bit(b->WB, m.from);
                set_bit(b->WB, m.to);
            }
            else{
                pop_bit(b->BB, m.from);
                set_bit(b->BB, m.to);
            }
            break;
        case ROOK:
            if(b->side == WHITE){
                //Añadimos info de enroque actual a unmake
                unmakeInfo.castle[0] = b->castle[0];
                unmakeInfo.castle[1] = b->castle[1];
                //Quitamos enroques
                if(get_bit(b->WR, m.from) == 56)b->castle[1] = 0;
                if(get_bit(b->WR, m.from) == 63)b->castle[0] = 0;
                pop_bit(b->WR, m.from);
                set_bit(b->WR, m.to);
            }
            else{
                //Añadimos info de enroque actual a unmake
                unmakeInfo.castle[0] = b->castle[2];
                unmakeInfo.castle[1] = b->castle[3];
                //Quitamos enroques
                if(get_bit(b->WR, m.from) == 0)b->castle[3] = 0;
                if(get_bit(b->WR, m.from) == 7)b->castle[2] = 0;
                pop_bit(b->BR, m.from);
                set_bit(b->BR, m.to);
            }
            break;
        case QUEEN:
            if(b->side == WHITE){
                pop_bit(b->WQ, m.from);
                set_bit(b->WQ, m.to);
            }
            else{
                pop_bit(b->BQ, m.from);
                set_bit(b->BQ, m.to);
            }
            break;
        case KING:
            if(b->side == WHITE){
                pop_bit(b->WK, m.from);
                set_bit(b->WK, m.to);
                //Enroque
                if(m.castling){
                    //Enroque corto
                    if(m.castling == 1){
                        if(b->castle[0]){
                            //Movemos torre
                            pop_bit(b->WR, 63);
                            set_bit(b->WR, 61);
                        }
                    }
                    //Enroque largo
                    if(m.castling == 2){
                        if(b->castle[1]) {
                            //Movemos torre
                            pop_bit(b->WR, 56);
                            set_bit(b->WR, 59);
                        }
                    }
                }
                //Añadimos info de enroque actual a unmake
                unmakeInfo.castle[0] = b->castle[0];
                unmakeInfo.castle[1] = b->castle[1];
                //Quitamos enroques
                b->castle[0] = 0;
                b->castle[1] = 0;
            }
            else{
                pop_bit(b->BK, m.from);
                set_bit(b->BK, m.to);
                //Enroque
                if(m.castling){
                    //Enroque corto
                    if(m.castling == 1){
                        if(b->castle[2]){
                            //Movemos torre
                            pop_bit(b->BR, 7);
                            set_bit(b->BR, 5);
                        }
                    }
                    //Enroque largo
                    if(m.castling == 2){
                        if(b->castle[3]) {
                            //Movemos torre
                            pop_bit(b->BR, 0);
                            set_bit(b->BR, 3);
                        }
                    }
                }
                //Añadimos info de enroque actual a unmake
                unmakeInfo.castle[0] = b->castle[2];
                unmakeInfo.castle[1] = b->castle[3];
                b->castle[2] = 0;
                b->castle[3] = 0;
            }
            break;
    }
    //Si hay captura borramos la pieza capturada
    if(m.capture){
        if(b->side == WHITE){
            if(get_bit(b->BP, m.to)){
                pop_bit(b->BP, m.to);
                unmakeInfo.capture_piece = PAWN;
            }
            else if(get_bit(b->BN, m.to)){
                pop_bit(b->BN, m.to);
                unmakeInfo.capture_piece = KNIGHT;
            }
            else if(get_bit(b->BB, m.to)){
                pop_bit(b->BB, m.to);
                unmakeInfo.capture_piece = BISHOP;
            }
            else if(get_bit(b->BR, m.to)){
                pop_bit(b->BR, m.to);
                unmakeInfo.capture_piece = ROOK;
            }
            else if(get_bit(b->BQ, m.to)){
                pop_bit(b->BQ, m.to);
                unmakeInfo.capture_piece = QUEEN;
            }
        }
        else{
            if(get_bit(b->WP, m.to)){
                pop_bit(b->WP, m.to);
                unmakeInfo.capture_piece = PAWN;
            }
            else if(get_bit(b->WN, m.to)){
                pop_bit(b->WN, m.to);
                unmakeInfo.capture_piece = KNIGHT;
            }
            else if(get_bit(b->WB, m.to)){
                pop_bit(b->WB, m.to);
                unmakeInfo.capture_piece = BISHOP;
            }
            else if(get_bit(b->WR, m.to)){
                pop_bit(b->WR, m.to);
                unmakeInfo.capture_piece = ROOK;
            }
            else if(get_bit(b->WQ, m.to)){
                pop_bit(b->WQ, m.to);
                unmakeInfo.capture_piece = QUEEN;
            }
        }
    }
    //Añadir a la pila de unmake la info
    push_unmake(unmakeStack, unmakeInfo);
    //Cambiar turno
    if(b->side == WHITE)b->side = BLACK;
    else b->side = WHITE;
}
//Hacer movimientos legales solamente
int make_legal_move(board *b, move m, unmake_stack *unmakeStack){
    make_move(b, m, unmakeStack);
if(b->side == BLACK){
        if(is_attacked(b, get_ls1b_index(b->WK), WHITE)) {
            unmake_move(b, m, unmakeStack);
            return 1;
        }
    }
    else{
    if(is_attacked(b, get_ls1b_index(b->BK), BLACK)) {
            unmake_move(b, m, unmakeStack);
            return 1;
        }
    }
    return 0;
}
//Deshacer movimiento
//Funciones para pila de unmake_info
void push_unmake(unmake_stack *stack, unmake_info unmake){
    stack->stack[stack->nElements].capture_piece = unmake.capture_piece;
    stack->stack[stack->nElements].capture_enpassant = unmake.capture_enpassant;
    stack->stack[stack->nElements].castle[0] = unmake.castle[0];
    stack->stack[stack->nElements].castle[1] = unmake.castle[1];
    stack->nElements++;
}
unmake_info pop_unmake(unmake_stack *stack){
    stack->nElements--;
    unmake_info unmake = stack->stack[stack->nElements];
    return unmake;
}
//Deshacer movimiento
void unmake_move(board *b, move m, unmake_stack *unmakeStack){
    //Cambiar turno
    if(b->side == WHITE)b->side = BLACK;
    else b->side = WHITE;
    struct unmake_info unmakeInfo = pop_unmake(unmakeStack);
    switch (m.piece) {
        case PAWN:
            if(b->side == WHITE){

                pop_bit(b->WP, m.to);
                set_bit(b->WP, m.from);
                //Si es una promoción transformar la pieza de nuevo
                if(m.promotion){
                    switch (m.promotion) {
                        case KNIGHT:
                            pop_bit(b->WN, m.to);
                            break;
                        case BISHOP:
                            pop_bit(b->WB, m.to);
                            break;
                        case ROOK:
                            pop_bit(b->WR, m.to);
                            break;
                        case QUEEN:
                            pop_bit(b->WQ, m.to);
                            break;
                    }
                }
                //Si hay captura al paso recuperamos la pieza capturada
                if(m.enpassant){
                    if(unmakeInfo.capture_enpassant == PAWN){
                        set_bit(b->BP, m.to + 8);
                        unmakeInfo.capture_enpassant = PAWN;
                    }
                    else if(unmakeInfo.capture_enpassant == KNIGHT){
                        set_bit(b->BN, m.to + 8);

                    }
                    else if(unmakeInfo.capture_enpassant == BISHOP){
                        set_bit(b->BB, m.to + 8);
                    }
                    else if(unmakeInfo.capture_enpassant == ROOK){
                        set_bit(b->BR, m.to + 8);
                        ;
                    }
                    else if(unmakeInfo.capture_enpassant == QUEEN){
                        set_bit(b->BQ, m.to + 8);
                    }
                }
            }
            else{
                set_bit(b->BP, m.from);
                pop_bit(b->BP, m.to);
                //Si es una promoción transformar la pieza de nuevo
                if(m.promotion){
                    switch (m.promotion) {
                        case KNIGHT:
                            pop_bit(b->BN, m.to);
                            break;
                        case BISHOP:
                            pop_bit(b->BB, m.to);
                            break;
                        case ROOK:
                            pop_bit(b->BR, m.to);
                            break;
                        case QUEEN:
                            pop_bit(b->BQ, m.to);
                            break;
                    }
                }
                //Si hay captura al paso recuperamos la pieza capturada
                if(m.enpassant){
                    if(unmakeInfo.capture_enpassant == PAWN){
                        set_bit(b->WP, m.to - 8);
                    }
                    else if(unmakeInfo.capture_enpassant == KNIGHT){
                        set_bit(b->WN, m.to - 8);
                    }
                    else if(unmakeInfo.capture_enpassant == BISHOP){
                        set_bit(b->WB, m.to - 8);
                    }
                    else if(unmakeInfo.capture_enpassant == ROOK){
                        set_bit(b->WR, m.to - 8);
                    }
                    else if(unmakeInfo.capture_enpassant == QUEEN){
                        set_bit(b->WQ, m.to - 8);
                    }
                }
            }
            break;
        case KNIGHT:
            if(b->side == WHITE){
                set_bit(b->WN, m.from);
                pop_bit(b->WN, m.to);
            }
            else{
                set_bit(b->BN, m.from);
                pop_bit(b->BN, m.to);
            }
            break;
        case BISHOP:
            if(b->side == WHITE){
                set_bit(b->WB, m.from);
                pop_bit(b->WB, m.to);
            }
            else{
                set_bit(b->BB, m.from);
                pop_bit(b->BB, m.to);
            }
            break;
        case ROOK:
            if(b->side == WHITE){
                //Recuperamos info de enroque actual de unmake
                b->castle[0] = unmakeInfo.castle[0];
                b->castle[1] = unmakeInfo.castle[1];
                set_bit(b->WR, m.from);
                pop_bit(b->WR, m.to);
            }
            else{
                //Recuperamos info de enroque actual de unmake
                b->castle[2] = unmakeInfo.castle[0];
                b->castle[3] = unmakeInfo.castle[1];
                set_bit(b->BR, m.from);
                pop_bit(b->BR, m.to);
            }
            break;
        case QUEEN:
            if(b->side == WHITE){
                set_bit(b->WQ, m.from);
                pop_bit(b->WQ, m.to);
            }
            else{
                set_bit(b->BQ, m.from);
                pop_bit(b->BQ, m.to);
            }
            break;
        case KING:
            if(b->side == WHITE){
                set_bit(b->WK, m.from);
                pop_bit(b->WK, m.to);
                //Enroque
                if(m.castling){
                    //Enroque corto
                    if(m.castling == 1){
                        if(b->castle[0]){
                            //Movemos torre
                            set_bit(b->WR, 63);
                            pop_bit(b->WR, 61);
                        }
                    }
                    //Enroque largo
                    if(m.castling == 2){
                        if(b->castle[1]) {
                            //Movemos torre
                            set_bit(b->WR, 56);
                            pop_bit(b->WR, 59);
                        }
                    }
                }
                //Recuperamos info de enroque actual
                b->castle[0] = unmakeInfo.castle[0];
                b->castle[1] = unmakeInfo.castle[1];

            }
            else{
                set_bit(b->BK, m.from);
                pop_bit(b->BK, m.to);
                //Enroque
                if(m.castling){
                    //Enroque corto
                    if(m.castling == 1){
                        if(b->castle[2]){
                            //Movemos torre
                            set_bit(b->BR, 7);
                            pop_bit(b->BR, 5);
                        }
                    }
                    //Enroque largo
                    if(m.castling == 2){
                        if(b->castle[3]) {
                            //Movemos torre
                            set_bit(b->BR, 0);
                            pop_bit(b->BR, 3);
                        }
                    }
                }
                //Recuperamos info de enroque actual
                b->castle[2] = unmakeInfo.castle[0];
                b->castle[3] = unmakeInfo.castle[1];
            }
            break;
    }
    //Si hay captura borramos la pieza capturada
    if(m.capture){
        if(b->side == WHITE){
            if(unmakeInfo.capture_piece == 0){
                set_bit(b->BP, m.to);
            }
            else if(unmakeInfo.capture_piece == KNIGHT){
                set_bit(b->BN, m.to);
            }
            else if(unmakeInfo.capture_piece == BISHOP){
                set_bit(b->BB, m.to);
            }
            else if(unmakeInfo.capture_piece == ROOK){
                set_bit(b->BR, m.to);
            }
            else if(unmakeInfo.capture_piece == QUEEN){
                set_bit(b->BQ, m.to);
            }
        }
        else{
            if(unmakeInfo.capture_piece == PAWN){
                set_bit(b->WP, m.to);
            }
            else if(unmakeInfo.capture_piece == KNIGHT){
                set_bit(b->WN, m.to);
            }
            else if(unmakeInfo.capture_piece == BISHOP){
                set_bit(b->WB, m.to);
            }
            else if(unmakeInfo.capture_piece == ROOK){
                set_bit(b->WR, m.to);
            }
            else if(unmakeInfo.capture_piece == QUEEN){
                set_bit(b->WQ, m.to);
            }
        }
    }
}

//Comprobar si una casilla está atacada por alguna pieza
int is_attacked(board *b, int square, unsigned side){
    //Bitboards que representa todas las piezas negras blancas y todas las negras
    black_pieces = b->BP | b->BN | b->BB | b->BR | b->BQ | b->BK;
    white_pieces = b->WP | b->WN | b->WB | b->WR | b->WQ | b->WK;
    any_pieces = black_pieces | white_pieces;
    
    //Si una pieza negra está atacando esta casilla
    if(side == BLACK){
        //Si la posición está atacada por un peón blanco(
        //(es lo mismo que si podriamos comer al peón con un peon negro desde nuestra casilla)
        if(pawn_attacks_table[BLACK][square] & b->WP) return 1;
        //Si está siendo atacado por caballos
        if(knight_move_table[square] & b->WN) return 1;

        //Incluimos la reina en torres y alfiles para simplificarlo
        //Si está siendo atacado por alfiles
        if(bishop_moves(square, any_pieces) & (b->WB | b->WQ)) return 1;
        //Si está siendo atacado por torres
        if(rook_moves(square, any_pieces)  & (b->WR | b->WQ)) return 1;
        //Si está siendo atacada por el rey
        if(king_move_table[square] & b->WK) return 1;
    }
    //Si una pieza blanca está atacando esta casilla
    else{
        //Si la posición está atacada por un peón negro(
        //(es lo mismo que si podriamos comer al peón con un peon blanco desde nuestra casilla)
        if(pawn_attacks_table[WHITE][square] & b->BP) return 1;
        //Si está siendo atacado por caballos
        if(knight_move_table[square] & b->BN) return 1;

        //Incluimos la reina en torres y alfiles para simplificarlo
        //Si está siendo atacado por alfiles
        if(bishop_moves(square, any_pieces) & (b->BB | b->BQ)) return 1;
        //Si está siendo atacado por torres
        if(rook_moves(square, any_pieces) & (b->BR | b->BQ)) return 1;
        //Si está siendo atacada por el rey
        if(king_move_table[square] & b->BK) return 1;
    }
    return 0;

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