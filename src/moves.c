#include "moves.h"
#include "zobrist.h"


unmake_stack stack;

//Constantes de bitboards equivalentes a partes del tablero utiles para hacer operaciones
const bitboard not_column_a = 18374403900871474942ULL;
const bitboard not_column_h = 9187201950435737471ULL;
const bitboard not_column_hg = 4557430888798830399ULL;
const bitboard not_column_ab = 18229723555195321596ULL;
const bitboard row_2 = 71776119061217280ULL;
const bitboard row_7 = 65280ULL;
const bitboard not_row_12 = 281474976710655ULL;
const bitboard not_row_78 = 18446744073709486080ULL;
bitboard black_pieces;
bitboard white_pieces;
bitboard any_pieces;

//Tablas con los movimientos de las piezas
bitboard knight_move_table[64];
bitboard king_move_table[64];
//En el caso de los peones cada color tiene sus propias tablas
bitboard pawn_pushes_table[2][64];
bitboard pawn_attacks_table[2][64];
bitboard pawn_promotion_pushes_table[2][64];
bitboard pawn_promotion_attacks_table[2][64];



//Añade elemento a la lista de elementos
static void addElement(moveList *l,unsigned char from, unsigned char  to, unsigned  capture, bitboard enpassantsquare,
                unsigned enpassant, unsigned  promotion, unsigned  castling, unsigned piece){
    unsigned short nElements = l->nElements;

    l->list[nElements].from = from;
    l->list[nElements].to = to;
    l->list[nElements].enpassantsquare = enpassantsquare;
    l->list[nElements].enpassant = enpassant;
    l->list[nElements].promotion = promotion;
    l->list[nElements].castling = castling;
    l->list[nElements].capture = capture;
    l->list[nElements].piece = piece;
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


//Funciones para pila de unmake_info

//Meter en la pila
static void push_unmake( unmake_info unmake){
    stack.stack[stack.nElements] = unmake;
    stack.nElements++;
}

//Sacar de ella
static unmake_info pop_unmake(){
    stack.nElements--;
    unmake_info unmake = stack.stack[stack.nElements];
    return unmake;
}
//Generar movimiento de los peones(Vamos a generarlos al inicio de la ejecución del engine)
static bitboard generate_pawn_pushes(int square, unsigned side){
    bitboard moves = 0ULL;

    bitboard board = 0ULL;

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

static bitboard generate_pawn_attacks(int square, unsigned side){
    bitboard moves = 0ULL;
    bitboard board = 0ULL;
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
static bitboard generate_pawn_promotion_pushes(int square, unsigned side){
    bitboard moves = 0ULL;

    bitboard board = 0ULL;

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

static bitboard generate_pawn_promotion_attacks(int square, unsigned side){
    bitboard moves = 0ULL;
    bitboard board = 0ULL;
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
static bitboard generate_knight_moves(int square){
    bitboard moves = 0ULL;

    bitboard board = 0ULL;
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
static bitboard generate_king_moves(int square){
    bitboard moves = 0ULL;
    bitboard board = 0ULL;
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


//Generar tablas para la generación de los movimientos
void generate_move_tables(){
    //Iniciamos tablas de magic bitboards
    init_sliders_attacks(1);
    init_sliders_attacks(0);

    //Generamos tabla de ataque de caballos, rey, peones
    for(int i = 0; i < 64; i++){
        knight_move_table[i] = generate_knight_moves(i);
        king_move_table[i] = generate_king_moves(i);
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


moveList *generate_black_moves(board *b, move lastMove, moveList *mL){
    //Posición inicial y final de la pieza movida
    unsigned char from;
    unsigned char to;
    //Bitboards que representa todas las piezas negras blancas y todas las negras
    black_pieces = b->BP | b->BN | b->BB | b->BR | b->BQ | b->BK;
    white_pieces = b->WP | b->WN | b->WB | b->WR | b->WQ | b->WK;
    any_pieces = black_pieces | white_pieces;

    bitboard aux;
    bitboard pawn_attacks;
    bitboard enpassant_attacks;
    bitboard promotion_pushes;
    bitboard promotion_attacks;
    //Peones negros
    bitboard BP = b->BP;
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
        if((from < 16) && ((~any_pieces>>(from+8))&1) && ((~any_pieces>>(from+16))&1)){
            to = from + 16;
            addElement(mL, from, to, 0, 1ULL<<to, 0, 0, 0, PAWN);
        }

        //Captura peones
        pawn_attacks = pawn_attacks_table[BLACK][from] & white_pieces;
        while(pawn_attacks){
            to = get_ls1b_index(pawn_attacks);
            addElement(mL, from, to, 1, 0, 0, 0, 0, PAWN);
            pop_bit(pawn_attacks, to);
        }

        if(lastMove.enpassantsquare){
            enpassant_attacks = pawn_attacks_table[BLACK][from-8] & lastMove.enpassantsquare;
            if(enpassant_attacks){
                to = get_ls1b_index(enpassant_attacks);
                addElement(mL, from, to + 8, 0, 0, 1, 0, 0, PAWN);
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
    bitboard BN = b->BN;
    while(BN){
        //Bitboard con los posibles movimientos de los caballos
        from = get_ls1b_index(BN);
        aux = knight_move_table[from]  & ~black_pieces;
        while(aux) {
            to = get_ls1b_index(aux);
            //Si captura pieza
            if(get_bit(white_pieces, to)) addElement(mL, from, to, 1, 0, 0, 0, 0, KNIGHT);
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
        if(get_bit(white_pieces, to)) addElement(mL, from, to, 1, 0, 0, 0, 0, KING);
        else addElement(mL, from, to, 0, 0, 0, 0, 0, KING);
        pop_bit(aux, to);
    }

    //Torres negras
    bitboard BR = b->BR;
    while(BR){
        from = get_ls1b_index(BR);
        //Calculamos los posibles movimientos de la torre
        aux = get_rook_attacks(from, any_pieces) & ~black_pieces;
        while(aux){
            to = get_ls1b_index(aux);
            //Si captura pieza
            if(get_bit(white_pieces, to)) addElement(mL, from, to, 1, 0, 0, 0, 0, ROOK);
            else addElement(mL, from, to, 0, 0, 0, 0, 0, ROOK);
            pop_bit(aux, to);
        }
        pop_bit(BR, from);
    }

    //Alfiles negros
    bitboard BB = b->BB;
    while(BB){
        from = get_ls1b_index(BB);
        //Calculamos los posibles movimientos de la torre
        aux = get_bishop_attacks(from, any_pieces) & ~black_pieces;
        while(aux){
            to = get_ls1b_index(aux);
            //Si captura pieza
            if(get_bit(white_pieces, to)) addElement(mL, from, to, 1, 0, 0, 0, 0, BISHOP);
            else addElement(mL, from, to, 0, 0, 0, 0, 0, BISHOP);
            pop_bit(aux, to);
        }
        pop_bit(BB, from);
    }

    //Reina negra
    bitboard BQ = b->BQ;
    while(BQ){
        from = get_ls1b_index(BQ);
        //Calculamos los movimientos de la reina
        aux = (get_bishop_attacks(from, any_pieces) | get_rook_attacks(from, any_pieces)) & ~black_pieces;
        while(aux){
            to = get_ls1b_index(aux);
            //Si captura pieza
            if( get_bit(white_pieces, to)) addElement(mL, from, to, 1, 0, 0, 0, 0, QUEEN);
            else addElement(mL, from, to, 0, 0, 0, 0, 0, QUEEN);
            pop_bit(aux, to);
        }
        pop_bit(BQ, from);
    }

    //Enroque negro corto
    if(b->castle[2]){
        //Si no hay ninguna pieza entre rey y torre y no están siendo atacadas las casillas
        if(!get_bit(any_pieces, 5) && !get_bit(any_pieces, 6)){
            if(!is_attacked(b, 4, BLACK) && !is_attacked(b, 5, BLACK) && !is_attacked(b, 6, BLACK)) {
                addElement(mL, 4, 6, 0, 0, 0, 0, 1, KING);
            }
        }
    }
    //Enroque largo negro
    if(b->castle[3]) {
        if (!get_bit(any_pieces, 1) && !get_bit(any_pieces, 2) && !get_bit(any_pieces, 3)) {
            if (!is_attacked(b, 2, BLACK) &&
                !is_attacked(b, 3, BLACK) && !is_attacked(b, 4, BLACK)) {
                addElement(mL, 4, 2, 0, 0, 0, 0, 2, KING);
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

    bitboard aux;
    bitboard pawn_attacks;
    bitboard enpassant_attacks;
    bitboard promotion_pushes;
    bitboard promotion_attacks;

    //Peones blancos
    bitboard WP = b->WP;
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
        if((from > 47) && ((~any_pieces>>(from-8))&1) && ((~any_pieces>>(from-16))&1)){
            to = from - 16;
            addElement(mL, from, to, 0, 1ULL<<to, 0, 0, 0, PAWN);
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
                addElement(mL, from, to - 8, 0, 0, 1, 0, 0, PAWN);
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
    bitboard WN = b->WN;
    while(WN){
        //Bitboard con los posibles movimientos de los caballos
        from = get_ls1b_index(WN);
        aux = knight_move_table[from]  & ~white_pieces;
        while(aux) {
            to = get_ls1b_index(aux);
            //Si captura pieza
            if(get_bit(black_pieces, to)) addElement(mL, from, to, 1, 0, 0, 0, 0, KNIGHT);
            else addElement(mL, from, to, 0, 0, 0, 0, 0, KNIGHT);
            pop_bit(aux, to);
        }
        pop_bit(WN, from);

    }

    //Rey blanco
    from = get_ls1b_index(b->WK);
    //Bitboard con los posibles movimientos del rey
    aux = king_move_table[from]  & ~white_pieces;
    while(aux) {
        to = get_ls1b_index(aux);
        //Si captura pieza
        if(get_bit(black_pieces, to)) addElement(mL, from, to, 1, 0, 0, 0, 0, KING);
        else addElement(mL, from, to, 0, 0, 0, 0, 0, KING);
        pop_bit(aux, to);
    }

    //Torres blancas
    bitboard WR = b->WR;
    while(WR){
        from = get_ls1b_index(WR);
        //Calculamos los posibles movimientos de la torre
        aux = get_rook_attacks(from, any_pieces) & ~white_pieces;
        while(aux){
            to = get_ls1b_index(aux);
            //Si captura pieza
            if(get_bit(black_pieces, to)) addElement(mL, from, to, 1, 0, 0, 0, 0, ROOK);
            else addElement(mL, from, to, 0, 0, 0, 0, 0, ROOK);
            pop_bit(aux, to);
        }
        pop_bit(WR, from);
    }

    //Alfiles blancos
    bitboard WB = b->WB;
    while(WB){
        from = get_ls1b_index(WB);
        //Calculamos los posibles movimientos de los alfiles
        aux = get_bishop_attacks(from, any_pieces) & ~white_pieces;
        while(aux){
            to = get_ls1b_index(aux);
            //Si captura pieza
            if(get_bit(black_pieces, to)) addElement(mL, from, to, 1, 0, 0, 0, 0, BISHOP);
            else addElement(mL, from, to, 0, 0, 0, 0, 0, BISHOP);
            pop_bit(aux, to);
        }
        pop_bit(WB, from);
    }

    //Reina blanca
    bitboard WQ = b->WQ;
    while(WQ){
        from = get_ls1b_index(WQ);
        //Calculamos los movimientos de la reina
        aux = (get_bishop_attacks(from, any_pieces) | get_rook_attacks(from, any_pieces)) & ~white_pieces;
        while(aux){
            to = get_ls1b_index(aux);
            //Si captura pieza
            if(get_bit(black_pieces, to)) addElement(mL, from, to, 1, 0, 0, 0, 0, QUEEN);
            else addElement(mL, from, to, 0, 0, 0, 0, 0, QUEEN);
            pop_bit(aux, to);
        }
        pop_bit(WQ, from);
    }

    //Enroque blanco corto
    if(b->castle[0]){
        //Si no hay ninguna pieza entre rey y torre y no están siendo atacadas las casillas
        if(!get_bit(any_pieces, 61) && !get_bit(any_pieces, 62)){
            if(!is_attacked(b, 60, WHITE) && !is_attacked(b, 61, WHITE) && !is_attacked(b, 62, WHITE) ) {
                addElement(mL, 60, 62, 0, 0, 0, 0, 1, KING);
            }
        }
    }
    //Enroque largo blanco
    if(b->castle[1]){
        //Si no hay ninguna pieza entre rey y torre y no están siendo atacadas las casillas
        if(!get_bit(any_pieces, 57) && !get_bit(any_pieces, 58) && !get_bit(any_pieces, 59)) {
            if (!is_attacked(b, 58, WHITE) &&
                !is_attacked(b, 59, WHITE) && !is_attacked(b, 60, WHITE)) {
                addElement(mL, 60, 58, 0, 0, 0, 0, 2, KING);
            }
        }
    }
    return mL;

}

//Realizar movimiento(no comprobamos si es legal o no en este momento)
void make_move(board *b, move m){
    unmake_info unmakeInfo;
    //Inicializamos valores de unmakeinfo
    unmakeInfo.castle[0] = 0;
    unmakeInfo.castle[1] = 0;
    unmakeInfo.castle[2] = 0;
    unmakeInfo.castle[3] = 0;
    unmakeInfo.capture_piece = 6;
    unmakeInfo.capture_enpassant = 6;
    unmakeInfo.enpassant_square = b->enpassant_square;
    b->enpassant_square = 0;
    //Actualizar hash de posibilidad de captura al paso
    if(m.enpassantsquare){
        b->hash ^= enpassant_keys[get_ls1b_index(m.enpassantsquare)];
    }
    //Actualizar hash pieza
    b->hash ^= piece_keys[m.from][m.piece][b->side];
    b->hash ^= piece_keys[m.to][m.piece][b->side];
    switch (m.piece) {
        case PAWN:
            if(b->side == WHITE){
                pop_bit(b->WP, m.from);
                set_bit(b->WP, m.to);
                if(m.enpassantsquare){
                    b->enpassant_square = m.enpassantsquare;
                }

                //Si es una promoción transformar la pieza
                if(m.promotion){
                    //Actualizar hash por promoción
                    b->hash ^= piece_keys[m.to][PAWN][b->side];
                    b->hash ^= piece_keys[m.to][m.promotion][b->side];
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
                        //Actualiza hash captura al paso
                        b->hash ^= piece_keys[m.to + 8][PAWN][BLACK];
                        pop_bit(b->BP, m.to + 8);
                        unmakeInfo.capture_enpassant = 1;
                }
            }
            else{
                pop_bit(b->BP, m.from);
                set_bit(b->BP, m.to);

                //Si es una promoción transformar la pieza
                if(m.promotion){
                    //Actualizar hash por promoción
                    b->hash ^= piece_keys[m.to][PAWN][b->side];
                    b->hash ^= piece_keys[m.to][m.promotion][b->side];
                    pop_bit(b->BP, m.to);
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
                        //Actualiza hash captura al paso
                        b->hash ^= piece_keys[m.to - 8][PAWN][WHITE];
                        pop_bit(b->WP, m.to - 8);
                        unmakeInfo.capture_enpassant = 1;
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
                if(m.from == 56){
                    b->castle[1] = 0;
                    //Actualizar hash enroque
                    b->hash ^= castle_keys[1];
                }
                if(m.from == 63){
                    b->castle[0] = 0;
                    //Actualizar hash enroque
                    b->hash ^= castle_keys[0];
                }
                pop_bit(b->WR, m.from);
                set_bit(b->WR, m.to);
            }
            else{
                //Añadimos info de enroque actual a unmake
                unmakeInfo.castle[2] = b->castle[2];
                unmakeInfo.castle[3] = b->castle[3];
                //Quitamos enroques
                if(m.from == 0){
                    b->castle[3] = 0;
                    //Actualizar hash enroque
                    b->hash ^= castle_keys[3];
                }
                if(m.from == 7){
                    b->castle[2] = 0;
                    //Actualizar hash enroque
                    b->hash ^= castle_keys[2];
                }
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
            if( b->side == WHITE){
                //Añadimos info de enroque actual a unmake
                unmakeInfo.castle[0] = b->castle[0];
                unmakeInfo.castle[1] = b->castle[1];
                pop_bit(b->WK, m.from);
                set_bit(b->WK, m.to);
                //Enroque
                if(m.castling){
                    //Enroque corto
                    if(m.castling == 1){
                        //Actualizar hash torre movida
                        b->hash ^= piece_keys[63][ROOK][b->side];
                        b->hash ^= piece_keys[61][ROOK][b->side];
                        //Movemos torre
                        pop_bit(b->WR, 63);
                        set_bit(b->WR, 61);
                    }
                    //Enroque largo
                    if(m.castling == 2){
                        //Actualizar hash torre movida
                        b->hash ^= piece_keys[56][ROOK][b->side];
                        b->hash ^= piece_keys[59][ROOK][b->side];
                            //Movemos torre
                            pop_bit(b->WR, 56);
                            set_bit(b->WR, 59);
                    }
                }

                //Actualizamos hash enroque
                b->hash ^= castle_keys[0];
                b->hash ^= castle_keys[1];
                //Quitamos enroques
                b->castle[0] = 0;
                b->castle[1] = 0;
            }
            else{
                //Añadimos info de enroque actual a unmake
                unmakeInfo.castle[2] = b->castle[2];
                unmakeInfo.castle[3] = b->castle[3];
                pop_bit(b->BK, m.from);
                set_bit(b->BK, m.to);
                //Enroque
                if(m.castling){
                    //Enroque corto
                    if(m.castling == 1){
                        //Actualizar hash torre movida
                        b->hash ^= piece_keys[7][ROOK][b->side];
                        b->hash ^= piece_keys[5][ROOK][b->side];
                            //Movemos torre
                            pop_bit(b->BR, 7);
                            set_bit(b->BR, 5);
                    }
                    //Enroque largo
                    if(m.castling == 2){
                        //Actualizar hash torre movida
                        b->hash ^= piece_keys[0][ROOK][b->side];
                        b->hash ^= piece_keys[3][ROOK][b->side];
                            //Movemos torre
                            pop_bit(b->BR, 0);
                            set_bit(b->BR, 3);
                    }
                }
                //Actualizamos hash enroque
                b->hash ^= castle_keys[2];
                b->hash ^= castle_keys[3];
                b->castle[2] = 0;
                b->castle[3] = 0;
            }
            break;
    }
    //Si hay captura borramos la pieza capturada
    if(m.capture){
        if(b->side == WHITE){
            if(get_bit(b->BP, m.to)){

                //Actualizamos hash pieza capturada
                b->hash ^= piece_keys[m.to][PAWN][BLACK];
                pop_bit(b->BP, m.to);
                unmakeInfo.capture_piece = PAWN;
            }
            else if(get_bit(b->BN, m.to)){
                //Actualizamos hash pieza capturada
                b->hash ^= piece_keys[m.to][KNIGHT][BLACK];
                pop_bit(b->BN, m.to);
                unmakeInfo.capture_piece = KNIGHT;
            }
            else if(get_bit(b->BB, m.to)){
                //Actualizamos hash pieza capturada
                b->hash ^= piece_keys[m.to][BISHOP][BLACK];
                pop_bit(b->BB, m.to);
                unmakeInfo.capture_piece = BISHOP;
            }
            else if(get_bit(b->BR, m.to)){
                //Actualizamos hash pieza capturada
                b->hash ^= piece_keys[m.to][ROOK][BLACK];
                pop_bit(b->BR, m.to);
                //Guardamos info acgtual de enroque y quitamos la posibilidad de enroque
                unmakeInfo.castle[2] = b->castle[2];
                unmakeInfo.castle[3] = b->castle[3];
                if(m.to == 0){
                    //Actualizamos hash pieza capturada enroque
                    b->hash ^= castle_keys[3];
                    b->castle[3] = 0;
                }
                if(m.to == 7){
                    //Actualizamos hash pieza capturada enroque
                    b->hash ^= castle_keys[2];
                    b->castle[2] = 0;
                }
                unmakeInfo.capture_piece = ROOK;
            }
            else if(get_bit(b->BQ, m.to)){
                //Actualizamos hash pieza capturada
                b->hash ^= piece_keys[m.to][QUEEN][BLACK];
                pop_bit(b->BQ, m.to);
                unmakeInfo.capture_piece = QUEEN;
            }
        }
        else{
            if(get_bit(b->WP, m.to)){
                pop_bit(b->WP, m.to);
                //Actualizamos hash pieza capturada
                b->hash ^= piece_keys[m.to][PAWN][WHITE];
                unmakeInfo.capture_piece = PAWN;
            }
            else if(get_bit(b->WN, m.to)){
                //Actualizamos hash pieza capturada
                b->hash ^= piece_keys[m.to][KNIGHT][WHITE];
                pop_bit(b->WN, m.to);
                unmakeInfo.capture_piece = KNIGHT;
            }
            else if(get_bit(b->WB, m.to)){
                //Actualizamos hash pieza capturada
                b->hash ^= piece_keys[m.to][BISHOP][WHITE];
                pop_bit(b->WB, m.to);
                unmakeInfo.capture_piece = BISHOP;
            }
            else if(get_bit(b->WR, m.to)){
                //Actualizamos hash pieza capturada
                b->hash ^= piece_keys[m.to][ROOK][WHITE];
                //Guardamos info acgtual de enroque y quitamos la posibilidad de enroque
                unmakeInfo.castle[0] = b->castle[0];
                unmakeInfo.castle[1] = b->castle[1];
                if(m.to == 56){
                    //Actualizamos hash pieza capturada enroque
                    b->hash ^= castle_keys[1];
                    b->castle[1] = 0;
                }
                if(m.to == 63){
                    //Actualizamos hash pieza capturada enroque
                    b->hash ^= castle_keys[0];
                    b->castle[0] = 0;
                }
                pop_bit(b->WR, m.to);
                unmakeInfo.capture_piece = ROOK;
            }
            else if(get_bit(b->WQ, m.to)){
                //Actualizamos hash pieza capturada
                b->hash ^= piece_keys[m.to][QUEEN][WHITE];
                pop_bit(b->WQ, m.to);
                unmakeInfo.capture_piece = QUEEN;
            }
        }
    }
    //Añadir a la pila de unmake la info
    push_unmake(unmakeInfo);
    //Cambiar turno
    if(b->side == WHITE)b->side = BLACK;
    else b->side = WHITE;
    //Actualizar hash turno
    b->hash ^= side_key;
}
//Hacer movimientos legales solamente
int make_legal_move(board *b, move m){
    make_move(b, m);
    if(b->side == BLACK){
        if(is_attacked(b, get_ls1b_index(b->WK), WHITE)) {
            unmake_move(b, m);
            return 1;
        }
    }
    else{
    if(is_attacked(b, get_ls1b_index(b->BK), BLACK)) {
            unmake_move(b, m);
            return 1;
        }
    }
    return 0;
}


//Deshacer movimiento
void unmake_move(board *b, move m){
    //Cambiar turno
    b->hash ^= side_key;
    if(b->side == WHITE)b->side = BLACK;
    else b->side = WHITE;
    //Actualizar hash de posibilidad de captura al paso
    if(m.enpassantsquare){
        b->hash ^= enpassant_keys[get_ls1b_index(m.enpassantsquare)];
    }
    //Actualizar hash turno
    struct unmake_info unmakeInfo = pop_unmake();
    b->enpassant_square = unmakeInfo.enpassant_square;
    //Actualizamos hash movida
    b->hash ^= piece_keys[m.to][m.piece][b->side];
    b->hash ^= piece_keys[m.from][m.piece][b->side];
    switch (m.piece) {
        case PAWN:
            if(b->side == WHITE){

                pop_bit(b->WP, m.to);
                set_bit(b->WP, m.from);
                //Si avanzamos dos casillas
                if(m.promotion){
                    //Actualizar hash por promoción
                    b->hash ^= piece_keys[m.to][PAWN][b->side];
                    b->hash ^= piece_keys[m.to][m.promotion][b->side];
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
                        //Actualiza hash captura al paso
                        b->hash ^= piece_keys[m.to + 8][PAWN][BLACK];
                        set_bit(b->BP, m.to + 8);
                }
            }
            else{
                set_bit(b->BP, m.from);
                pop_bit(b->BP, m.to);
                //Si es una promoción transformar la pieza de nuevo
                if(m.promotion){
                    //Actualizar hash por promoción
                    b->hash ^= piece_keys[m.to][PAWN][b->side];
                    b->hash ^= piece_keys[m.to][m.promotion][b->side];
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
                    //Actualiza hash captura al paso
                    b->hash ^= piece_keys[m.to - 8][PAWN][WHITE];
                        set_bit(b->WP, m.to - 8);
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
                if(m.from == 56){
                    //Actualizar hash enroque
                    b->hash ^= castle_keys[1];
                }
                if(m.from == 63){
                    //Actualizar hash enroque
                    b->hash ^= castle_keys[0];
                }
                set_bit(b->WR, m.from);
                pop_bit(b->WR, m.to);
            }
            else{
                //Recuperamos info de enroque actual de unmake
                b->castle[2] = unmakeInfo.castle[2];
                b->castle[3] = unmakeInfo.castle[3];
                if(m.from == 0){
                    //Actualizar hash enroque
                    b->hash ^= castle_keys[3];
                }
                if(m.from == 7){
                    //Actualizar hash enroque
                    b->hash ^= castle_keys[2];
                }
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

                //Recuperamos info de enroque actual
                b->castle[0] = unmakeInfo.castle[0];
                b->castle[1] = unmakeInfo.castle[1];
                set_bit(b->WK, m.from);
                pop_bit(b->WK, m.to);
                //Enroque
                if(m.castling){
                    //Enroque corto
                    if(m.castling == 1){
                        //Actualizar hash torre movida
                        b->hash ^= piece_keys[63][ROOK][b->side];
                        b->hash ^= piece_keys[61][ROOK][b->side];
                            //Movemos torre
                            set_bit(b->WR, 63);
                            pop_bit(b->WR, 61);
                    }
                    //Enroque largo
                    if(m.castling == 2){

                        //Actualizar hash torre movida
                        b->hash ^= piece_keys[56][ROOK][b->side];
                        b->hash ^= piece_keys[59][ROOK][b->side];
                            //Movemos torre
                            set_bit(b->WR, 56);
                            pop_bit(b->WR, 59);
                    }
                }
                //Actualizamos hash enroque
                b->hash ^= castle_keys[0];
                b->hash ^= castle_keys[1];

            }
            else{
                //Recuperamos info de enroque actual
                b->castle[2] = unmakeInfo.castle[2];
                b->castle[3] = unmakeInfo.castle[3];
                set_bit(b->BK, m.from);
                pop_bit(b->BK, m.to);
                //Enroque
                if(m.castling){
                    //Enroque corto
                    if(m.castling == 1){
                        //Actualizar hash torre movida
                        b->hash ^= piece_keys[7][ROOK][b->side];
                        b->hash ^= piece_keys[5][ROOK][b->side];
                            //Movemos torre
                            set_bit(b->BR, 7);
                            pop_bit(b->BR, 5);
                        }
                    }
                    //Enroque largo
                    if(m.castling == 2){
                        //Actualizar hash torre movida
                        b->hash ^= piece_keys[0][ROOK][b->side];
                        b->hash ^= piece_keys[3][ROOK][b->side];
                            //Movemos torre
                            set_bit(b->BR, 0);
                            pop_bit(b->BR, 3);

                }
                //Actualizamos hash enroque
                b->hash ^= castle_keys[2];
                b->hash ^= castle_keys[3];
            }

            break;
    }
    //Si hay captura recuperamos la pieza capturada
    if(m.capture){

        if(b->side == WHITE){
            //Actualizamos hash pieza capturada
            b->hash ^= piece_keys[m.to][unmakeInfo.capture_piece][BLACK];
            if(unmakeInfo.capture_piece == PAWN){
                set_bit(b->BP, m.to);
            }
            else if(unmakeInfo.capture_piece == KNIGHT){
                set_bit(b->BN, m.to);
            }
            else if(unmakeInfo.capture_piece == BISHOP){
                set_bit(b->BB, m.to);
            }
            else if(unmakeInfo.capture_piece == ROOK){
                if(m.to == 0){
                    //Actualizamos hash pieza capturada enroque
                    b->hash ^= castle_keys[3];
                }
                if(m.to == 7){
                    //Actualizamos hash pieza capturada enroque
                    b->hash ^= castle_keys[2];
                }
                //Recuperar info de enroque
                b->castle[2] = unmakeInfo.castle[2];
                b->castle[3] = unmakeInfo.castle[3];
                set_bit(b->BR, m.to);
            }
            else if(unmakeInfo.capture_piece == QUEEN){
                set_bit(b->BQ, m.to);
            }
        }
        else{
            //Actualizamos hash pieza capturada
            b->hash ^= piece_keys[m.to][unmakeInfo.capture_piece][WHITE];
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
                if(m.to == 56){
                    //Actualizamos hash pieza capturada enroque
                    b->hash ^= castle_keys[1];
                }
                if(m.to == 63){
                    //Actualizamos hash pieza capturada enroque
                    b->hash ^= castle_keys[0];
                }
                //Recuperar info de enroque
                set_bit(b->WR, m.to);
                b->castle[0] = unmakeInfo.castle[0];
                b->castle[1] = unmakeInfo.castle[1];
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
        if(get_bishop_attacks(square, any_pieces) & (b->WB | b->WQ)) return 1;
        //Si está siendo atacado por torres
        if(get_rook_attacks(square, any_pieces)  & (b->WR | b->WQ)) return 1;
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
        if(get_bishop_attacks(square, any_pieces) & (b->BB | b->BQ)) return 1;
        //Si está siendo atacado por torres
        if(get_rook_attacks(square, any_pieces) & (b->BR | b->BQ)) return 1;
        //Si está siendo atacada por el rey
        if(king_move_table[square] & b->BK) return 1;
    }
    return 0;

}



//Convertir movimiento(notación en forma de origen + destino)
//Falta promocion
void move_to_string(move *m, char *string){

    //Origen
    *string++ = 'a' + (m->from % 8);
    *string++  = '1' + 7 - (m->from / 8);
    //Destino
    *string++ = 'a' + (m->to % 8);
    *string++ = '1' + 7 - (m->to / 8);

    if(m->promotion){
        if(m->promotion == 1){
            *string = 'n';
        }
        if(m->promotion == 2){
            *string = 'b';
        }
        if(m->promotion == 3){
            *string = 'r';
        }
        if(m->promotion == 4){
            *string = 'q';
        }
    }
    else{
        *string = '\0';
    }

}

void string_to_move(char *string, move *m, board *b){

    char *work = string;

    //Origen
    m->from = (*work++ - 'a') + (('1' + 7 -*work++)* 8);
    //Destino
    m->to = (*work++ - 'a') + (('1' + 7 - *work++) * 8);

    //Enroque largo
    if((get_bit(b->WK, m->from) || get_bit(b->BK, m->from)) && (m->from - m->to)== 2){
        m->castling = 2;
    }
    //Enroque corto
    if((get_bit(b->WK, m->from) || get_bit(b->BK, m->from)) && (m->to - m->from)== 2){
        m->castling = 1;
    }
    m->piece = piece_square(m->from, b);
    m->capture = (piece_square(m->to, b) != -1) ? 1 : 0;
    m->enpassant = (b->enpassant_square == m->to) ? 1 : 0;
    switch (string[4]) {
        case 'q':
            m->promotion = QUEEN;
            break;
        case 'r':
            m->promotion = ROOK;
            break;
        case 'b':
            m->promotion = BISHOP;
            break;
        case 'n':
            m->promotion = KNIGHT;
            break;
    }

}