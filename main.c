#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "moves.h"
int main() {
    //Generar un tablero con la partida empezada e imprimirlo
    board board;
    initBoard(&board);
    printBoard(board);

    moveList *m;
    m = generate_white_moves(&board);
    char* string = malloc(sizeof(char) * 4);
    move_to_string(&m->list[0], string);    
    printf("Movimiento blancas: %s ", string);
    for(int i = 1; i < m->nElements; i++){
      move_to_string(&m->list[i], string);        
      printf(", %s", string);
    }
    printf(".\n");
    m = generate_black_moves(&board);
    move_to_string(&m->list[0], string);    
    printf("Movimiento negras: %s", string);
    for(int i = 1; i < m->nElements; i++){
      move_to_string(&m->list[i], string);        
      printf(", %s", string);
    }
    printf(".\n");
    return 0;
}
