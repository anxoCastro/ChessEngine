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
unsigned long black_pieces;
unsigned long white_pieces;
unsigned long any_pieces;
unsigned long knight_move_table[64];
unsigned long king_move_table[64];
unsigned long rook_mask_table[64];
unsigned long bishop_mask_table[64];

//Inicializa una lista de movimientos
moveList *create_move_list(){
    moveList *list = malloc(sizeof(moveList));
    list->nElements = 0;
    return list;
}

//Añade elemento a la lista de elementos
void addElement(moveList *l, move m){
    l->list[l->nElements].from = m.from;
    l->list[l->nElements].to = m.to;
    l->list[l->nElements].castlingsquare = m.castlingsquare;
    l->nElements++;
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
    if(b->side){
        generate_black_moves(b, lastMove);
    }
    else{
        generate_white_moves(b, lastMove);
    }
}

moveList *generate_black_moves(board *b, move lastMove){
    //Creamos la lista
    moveList *mL = create_move_list();
    move m;
    //Bitboards que representa todas las piezas negras blancas y todas las negras
    black_pieces = b->BP | b->BN | b->BB | b->BR | b->BQ | b->BK;
    white_pieces = b->WP | b->WN | b->WB | b->WR | b->WQ | b->WK;
    any_pieces = black_pieces | white_pieces;
    //Peones negros en la segunda fila que podremos avanzar dos casillas
    unsigned long black_7 = b->BP & row_7;
    //Peones negros
    //Recorremos todas las posiciones de la bit board menos la fila 8
    for(unsigned char i = 0; i < 56; i++){
        //Peones negros
        //Un movimiento hacia delante
        if( (((b->BP >> i)&1)==1) && (!((any_pieces >> i + 8)&1)==1) ){
            m.from = i;
            m.to = i + 8;
            m.castlingsquare = 0;
            addElement(mL, m);
            //Dos movimientos hacia adelante
            if( (((black_7 >> i)&1) == 1) && (!((any_pieces >> i + 16)&1) == 1)){
                m.from = i;
                m.to = i + 16;
                //Indicar que se puede hacer captura al paso después de este movimiento
                m.castlingsquare = 1UL << i;
                addElement(mL, m);
            }
        }
        
        //Comer pieza a la derecha
        if( (((b->BP >> i)&1)==1) && (((white_pieces >> (i+9))&1) == 1) ){
            m.from = i;
            m.to = i+ 9;
            m.castlingsquare = 0;
            addElement(mL, m);
        }
        //Comer pieza a la izquierda
        if( (((b->BP >> i)&1)==1) && (((white_pieces >> (i+7))&1) == 1) ){
            m.from = i;
            m.to = i + 7;
            m.castlingsquare = 0;
            addElement(mL, m);
        }
    }
    //Captura al paso
    if(lastMove.castlingsquare){
        int objetivo = floor_log2(lastMove.castlingsquare);
        //Comer a la izquierda
        if((b->BP >> (objetivo-1)&1) == 1){
            m.from = objetivo-1;
            m.to = objetivo+7;
            m.castlingsquare = 0;
            m.castling = 1;
            addElement(mL, m);
        }
        //Comer a la derecha
        if((b->BP >> (objetivo+1)&1) == 1){
            m.from = objetivo+1;
            m.to = objetivo+9;
            m.castlingsquare = 0;
            m.castling = 1;
            addElement(mL, m);
        }
    }

    //Restablecemos valores de captura al paso para el resto de movimientos
    m.castling = 0;
    m.castlingsquare = 0;
    unsigned long aux;
    //Caballos negros
    unsigned long BN = b->BN;
    while(BN){
        //Bitboard con los posibles movimientos de los caballos
        m.from = get_ls1b_index(BN);
        aux = knight_move_table[m.from]  & ~black_pieces;
        while(aux) {
            m.to = get_ls1b_index(aux);
            addElement(mL, m);
            pop_bit(aux, m.to);
        }
        pop_bit(BN, m.from);

    }

    //Rey negro
    m.from = get_ls1b_index(b->BK);
    //Consultamos los movimientos y descartamos aquellos que no se pueden hacer por haber una pieza negra
    aux = king_move_table[m.from]  & ~black_pieces;
    while(aux) {
        m.to = get_ls1b_index(aux);
        addElement(mL, m);
        pop_bit(aux, m.to);
    }

    /*Aquí usaríamos magic bitboards para calcular los movimientos pero por ahora voy a dejarlo así*/
    //Torres negras
    unsigned long BR = b->BR;
    while(BR){
        m.from = get_ls1b_index(BR);
        //Calculamos los posibles movimientos de la torre
        aux = rook_moves(m.from, any_pieces) & ~black_pieces;
        while(aux){
            m.to = get_ls1b_index(aux);
            addElement(mL, m);
            pop_bit(aux, m.to);
        }
        pop_bit(BR, m.from);
    }

    //Alfiles negros
    unsigned long BB = b->BB;
    while(BB){
        m.from = get_ls1b_index(BB);
        //Calculamos los posibles movimientos de la torre
        aux = bishop_moves(m.from, any_pieces) & ~black_pieces;
        while(aux){
            m.to = get_ls1b_index(aux);
            addElement(mL, m);
            pop_bit(aux, m.to);
        }
        pop_bit(BB, m.from);
    }

    //Reina negra
    if(b->BQ){
        m.from = get_ls1b_index(b->BQ);
        //Calculamos los movimientos de la reina
        aux = (bishop_moves(m.from, any_pieces) | rook_moves(m.from, any_pieces)) & ~black_pieces;
        while(aux){
            m.to = get_ls1b_index(aux);
            addElement(mL, m);
            pop_bit(aux, m.to);
        }
    }
    return mL;

}
moveList *generate_white_moves(board *b, move lastMove){
    //Creamos la lista
    moveList *mL = create_move_list();

    //Bitboards que representa todas las piezas negras blancas y todas las negras
    black_pieces = b->BP | b->BN | b->BB | b->BR | b->BQ | b->BK;
    white_pieces = b->WP | b->WN | b->WB | b->WR | b->WQ | b->WK;
    any_pieces = black_pieces | white_pieces;

    //White pawns
    //Recorremos todas las posiciones de la bit board menos la fila 8
    move m;
    //Peones blancos en la septima fila que podremos avanzar dos casillas
    unsigned long white_2 = b->WP & row_2;
    for(unsigned char i = 8; i < 64; i++){
        //Peones blancos
        //Un movimiento hacia delante
        if( (((b->WP>> i)&1)==1) && (!((any_pieces >> i - 8)&1)==1) ){
            m.from = i;
            m.to = i - 8;
            m.castlingsquare = 0;
            addElement(mL, m);
            //Dos movimientos hacia adelante
            if( (((white_2 >> i)&1) == 1) && (!((any_pieces >> i - 16)&1) == 1)){
                m.from = i;
                m.to = i - 16;
                //Indicar que se puede hacer captura al paso después de este movimiento
                m.castlingsquare = 1UL >> i;
                addElement(mL, m);
            }
        }
        //Comer pieza a la derecha
        if( (((b->WP >> i)&1)==1) && (((black_pieces >> (i-9))&1) == 1) ){
            m.from = i;
            m.to = i - 9;
            m.castlingsquare = 0;
            addElement(mL, m);
        }
        //Comer pieza a la izquierda
        if( (((b->WP >> i)&1)==1) && (((black_pieces >> (i-7))&1) == 1) ){
            m.from = i;
            m.to = i - 7;
            m.castlingsquare = 0;
            addElement(mL, m);
        }
    }
    //Captura al paso
    if(lastMove.castlingsquare){
        int objetivo = floor_log2(lastMove.castlingsquare);
        //Comer a la izquierda
        if((b->WP >> (objetivo-1)&1) == 1){
            m.from = objetivo-1;
            m.to = objetivo-7;
            m.castlingsquare = 0;
            m.castling = 1;
            addElement(mL, m);
        }
        //Comer a la derecha
        if((b->BP >> (objetivo+1)&1) == 1){
            m.from = objetivo+1;
            m.to = objetivo-9;
            m.castlingsquare = 0;
            m.castling = 1;
            addElement(mL, m);
        }
    }
    //Restablecemos valores de captura al paso para el resto de movimientos
    m.castling = 0;
    m.castlingsquare = 0;
    unsigned long aux;

    //Caballos blancos
    unsigned long WN = b->WN;
    while(WN){
        //Bitboard con los posibles movimientos de los caballos
        m.from = get_ls1b_index(WN);
        aux = knight_move_table[m.from]  & ~white_pieces;
        while(aux) {
            m.to = get_ls1b_index(aux);
            addElement(mL, m);
            pop_bit(aux, m.to);
        }
        pop_bit(WN, m.from);

    }

    //Rey blanco
    m.from = get_ls1b_index(b->WK);
    //Consultamos los movimientos y descartamos aquellos que no se pueden hacer por haber una pieza negra
    aux = king_move_table[m.from]  & ~white_pieces;
    while(aux) {
        m.to = get_ls1b_index(aux);
        addElement(mL, m);
        pop_bit(aux, m.to);
    }


    /*Aquí usaríamos magic bitboards para calcular los movimientos pero por ahora voy a dejarlo así*/
    //Torres blancas
    unsigned long WR = b->WR;
    while(WR){
        m.from = get_ls1b_index(WR);
        //Calculamos los posibles movimientos de la torre
        aux = rook_moves(m.from, any_pieces) & ~white_pieces;
        while(aux){
            m.to = get_ls1b_index(aux);
            addElement(mL, m);
            pop_bit(aux, m.to);
        }
        pop_bit(WR, m.from);
    }

    //Alfiles blancos
    unsigned long WB = b->WB;
    while(WB){
        m.from = get_ls1b_index(WB);
        //Calculamos los posibles movimientos de la torre
        aux = bishop_moves(m.from, any_pieces) & ~white_pieces;
        while(aux){
            m.to = get_ls1b_index(aux);
            addElement(mL, m);
            pop_bit(aux, m.to);
        }
        pop_bit(WB, m.from);
    }

    //Reina blanca
    if(b->WQ){
        m.from = get_ls1b_index(b->WQ);
        //Calculamos los movimientos de la reina
        aux = (bishop_moves(m.from, any_pieces) | rook_moves(m.from, any_pieces)) & ~white_pieces;
        while(aux){
            m.to = get_ls1b_index(aux);
            addElement(mL, m);
            pop_bit(aux, m.to);
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