#include <stdio.h>
#include "board.h"
int main() {
    board board;
    initBoard(&board);
    printBoard(board);
    return 0;
}
