#include <stdio.h>
#include <stdlib.h>
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
    moveList *m;
    m = generate_legal_moves(&board);
    char* string = malloc(sizeof(char) * 4);
    for(int i = 0; i < m->nElements; i++){
      move_to_string(&m->list[i], string);        

      printf("%s\n", string);
    }
    return 0;
}
