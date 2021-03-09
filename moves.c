//
// Created by anxo on 07/05/20.
//

#include "moves.h"
#include "board.h"
#include <stdio.h>
#include <stdlib.h>

//Constantes de bitboards equivalentes a partes del tablero utiles para hacer operaciones
const unsigned long column_a = 72340172838076673UL;
const unsigned long column_h = 9259542123273814144UL;
const unsigned long not_column_a = 18374403900871474942ULL;
const unsigned long not_column_h = 9187201950435737471ULL;
const unsigned long not_column_hg = 4557430888798830399ULL;
const unsigned long not_column_ab = 18229723555195321596ULL;
const unsigned long row_1 = 18374686479671623680UL;
const unsigned long row_2 = 71776119061217280UL;
const unsigned long row_7 = 65280UL;
const unsigned long row_8 = 255UL;
unsigned long black_pieces;
unsigned long white_pieces;
unsigned long any_pieces;
unsigned long knight_move_table[64];
moveList *create_move_list(){
    moveList *list = malloc(sizeof(moveList));
    list->nElements = 0;
    return list;
}

void addElement(moveList *l, move m){
    l->list[l->nElements].from = m.from;
    l->list[l->nElements].to = m.to;
    l->list[l->nElements].castlingsquare = m.castlingsquare;
    l->nElements++;
}

unsigned long generate_knight_move(int square){
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

void generate_move_tables(){
    //Generamos tabla de ataque de caballos, peones
    for(int i = 0; i < 64; i++){
        knight_move_table[i] = generate_knight_move(i);
    }
}
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

    //Bitboards que representa todas las piezas negras blancas y todas las negras
    black_pieces = b->BP | b->BN | b->BB | b->BR | b->BQ | b->BK;
    white_pieces = b->WP | b->WN | b->WB | b->WR | b->WQ | b->WK;
    any_pieces = black_pieces | white_pieces;
    //Black pawns
    //Recorremos todas las posiciones de la bit board menos la fila 8
    move m;
    //Peones blancos en la segunda fila que podremos avanzar dos casillas
    unsigned long black_7 = b->BP & row_7;
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

    //Caballos negros
    unsigned long aux;
    unsigned long BN = b->BN;
    m.castling = 0;
    m.castlingsquare = 0;
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

    //Caballos blancos
    unsigned long aux;
    unsigned long WN = b->WN;
    m.castling = 0;
    m.castlingsquare = 0;
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
    return mL;

}

void print_bitboard(unsigned long b) {
    char charBoard[8][8] = {[0 ... 7][0 ... 7] = '-'};
    for (int i = 0; i < 64; i++) {
        if(((b>>i)&1)==1) {
            charBoard[i / 8][i % 8] = '+';
        }
    }
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