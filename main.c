#include <stdio.h>
#include "board.h"
#include "moves.h"
int main() {
    //Generar un tablero con la partida empezada e imprimirlo
    board board;
    initBoard(&board);
    printBoard(board);

    //Imprimir bitboard fila a
    print_bitboard(row_a);
    //Imprimir fila h
    print_bitboard(row_h);
    return 0;
}
