/*
Most of the code is written by me. 
However the - '0' at the convert notation function and
the memcpy(movements, Piece_Movements[board[src]->piece->type], 8 * sizeof(int)) at the can_move function 
was written after a correction copilot made.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define EDGE_LEN 8

// int Piece_Scores[6] = {100, 300, 350, 500, 900, 999999999};

int Piece_MovePowers[6] = {1, 1, EDGE_LEN, EDGE_LEN, EDGE_LEN, 1};

int Piece_Movements[6][8] = {{
    // Pawn
    EDGE_LEN}
    , {
    // Knight
    2 + EDGE_LEN, 2 - EDGE_LEN,
    -2 + EDGE_LEN, -2 - EDGE_LEN,
    EDGE_LEN * 2 + 1, EDGE_LEN * 2 - 1,
    -EDGE_LEN * 2 + 1, -EDGE_LEN * 2 - 1
    }, {
    // Bishop
    EDGE_LEN + 1, EDGE_LEN - 1, 
    -EDGE_LEN + 1, -EDGE_LEN - 1}, {
    // Rook
    1, -1,
    EDGE_LEN,
    -EDGE_LEN}, {
    // Queen
    1, -1,
    EDGE_LEN, -EDGE_LEN, 
    EDGE_LEN + 1, EDGE_LEN - 1, 
    -EDGE_LEN + 1, -EDGE_LEN - 1}, {
    // King
    1, -1,
    EDGE_LEN, -EDGE_LEN, 
    EDGE_LEN + 1, EDGE_LEN - 1, 
    -EDGE_LEN + 1, -EDGE_LEN - 1}
    };

char Piece_Symbols[12] = {'P', 'N', 'B', 'R', 'Q', 'K', 'p', 'n', 'b', 'r', 'q', 'k'};

enum Piece_Color {
    White,
    Black
};

enum Piece_Type {
    Pawn,
    Knight,
    Bishop,
    Rook,
    Queen,
    King
};

struct Piece {
    int last_move;
    enum Piece_Color color;
    enum Piece_Type type;
    struct Square* square;
};

struct Square {
    int id;
    int color;
    struct Piece* piece;
};

int move = 1;
int is_move_legal = 0;
enum Piece_Color turn = White;

struct Piece* new_piece(enum Piece_Type type, enum Piece_Color color, struct Square* square) {
    struct Piece* new_piece = (struct Piece*)malloc(sizeof(struct Piece));

    new_piece->type = type;
    new_piece->color = color;

    new_piece->square = square;

    new_piece->last_move = 0;

    return new_piece;
}

struct Square* new_square(int id, int color) {
    struct Square* new_square = (struct Square*)malloc(sizeof(struct Square));

    new_square->id = id;
    new_square->color = color;

    new_square->piece = NULL;

    return new_square;
}

struct Square** create_board(struct Square** board) {
    for (int i = 0; i < EDGE_LEN * EDGE_LEN; i++) {
        board[i] = new_square(i, i % 2? 256 : 0);
    }

    return board;
}

int can_move(struct Square** board, int src, int dest) {
    if (src == dest) {
        return 0;
    } else if (0 > dest || dest >= EDGE_LEN * EDGE_LEN) {
        return 0;
    } else if (!board[src]->piece) {
        return 0;
    } else if ((board[dest]->piece && board[src]->piece->color == board[dest]->piece->color)) {
        return 0;
    } else if (board[src]->piece->color != turn) {
        return 0;
    }

    int temp;
    int* move_powers = (int*)calloc(6, sizeof(int));
    int* movements = (int*)calloc(8, sizeof(int));

    memcpy(move_powers, Piece_MovePowers, 6 * sizeof(int));
    memcpy(movements, Piece_Movements[board[src]->piece->type], 8 * sizeof(int));

    if (board[src]->piece->type == Pawn) { 
        movements[0] *= board[src]->piece->color * 2 - 1;

        if (board[dest]->piece) {
            return board[src + movements[0] + 1] == board[dest] || board[src + movements[0] - 1] == board[dest];
        } 
        else if (EDGE_LEN - src / EDGE_LEN - 1 == board[src]->piece->color * 5 + 1) {
            move_powers[0] = 2;
        }
    }

    int rook_distance = 3;
    if (board[src]->piece->type == King) {
        if (src + 2 == dest) { 
            if(board[src + 3]->piece->type == Rook && board[src + 3]->piece->last_move == 0 && board[src]->piece->last_move == 0) {
                move_powers[5] = 2;
            } else {
                return 0;
            }
        } else if (src - 2 == dest ) {
            if (board[src - 4]->piece->type == Rook && board[src - 4]->piece->last_move == 0 && board[src]->piece->last_move == 0) {
                move_powers[5] = 2;
                rook_distance = -4;
            }  else {
                return 0;
            }
        }
    }

    for (int j = 1; j <= move_powers[board[src]->piece->type]; j++) {
        for (int i = 0; i < 8; i++) {
            if (!movements[i]) {
                continue;
            }

            if (src + movements[i] * j == dest) {

                if (move_powers[5] == 2) {
                    board[src + rook_distance]->piece->last_move = move;
                    board[src + (rook_distance > 0 ? 1 : -1)]->piece = board[src + rook_distance]->piece;
                    board[src + rook_distance]->piece = NULL;
                }

                if (abs(movements[i]) > 1) {
                    if (abs(movements[i]) > 9) {
                        temp = movements[i] > 0 ? (movements[i] - EDGE_LEN * 2) : (movements[i] + EDGE_LEN * 2);
                    } else {
                        temp = movements[i] > 0 ? (movements[i] - EDGE_LEN) : (movements[i] + EDGE_LEN);
                    }
                } else {
                    temp = movements[i];
                }

                if (temp != 0) {
                    if (temp > 0 && (src % EDGE_LEN >= dest % EDGE_LEN)) {
                        return 0;
                    }
                    if (temp < 0 && (src % EDGE_LEN <= dest % EDGE_LEN)) {
                        return 0;
                    }
                }

                free(movements);
                free(move_powers);

                return 1;
            }

            if (0 > src + movements[i] * j || src + movements[i] * j >= EDGE_LEN * EDGE_LEN) {
                continue;
            }

            if (board[src + movements[i] * j]->piece) {
                movements[i] = 0;
            }
        }
    }

    free(movements);
    free(move_powers);

    return 0;
}

struct Square** move_piece(struct Square** board, int src, int dest) {
    if (can_move(board, src, dest)) {
        board[src]->piece->last_move = move;
        board[dest]->piece = board[src]->piece;
        board[src]->piece = NULL;
        is_move_legal = 1;
    } else {
        printf("\nIllegal move!\n\n");
        is_move_legal = 0;
    }

    return board;
}

void draw_board(struct Square** board) {
    for (int i = 0; i < EDGE_LEN; i++) {
        for (int j = 0; j < EDGE_LEN; j++) {
            printf("%c", board[i * EDGE_LEN + j]->piece ? Piece_Symbols[board[i * EDGE_LEN + j]->piece->type + board[i * EDGE_LEN + j]->piece->color * 6]: '-');
            printf(" ");
        }
        printf("\n");
    }
}

int convert_notation(char* pos) {
    return (*pos - 'a') + (EDGE_LEN - (*(pos + 1) - '0' - 1) - 1) * EDGE_LEN;
}

int find_index(char* arr, char target) {
    for (int i = 0; i < EDGE_LEN * EDGE_LEN; i++) {
        if (arr[i] == target) {
            return i;
        }
    }
    return -1;
}

struct Square** set_board_position(struct Square** board, char* fen) {
    int i = 0;
    int j = 0;
    int temp;
    for (; i < strlen(fen); i++) { 
        if (fen[i] > '0' - 1 && fen[i] < '9' + 1) {
            temp = j + fen[i] - '0';
            for (; j < temp; j++) {
                board[j]->piece = NULL;
            }
        } else if (fen[i] > 'A' - 1 && fen[i] < 'Z' + 1) {
            board[j]->piece = new_piece(find_index(Piece_Symbols, fen[i]), White, board[j]);
            j++;
        } else if (fen[i] > 'a' - 1 && fen[i] < 'z' + 1) {
            board[j]->piece = new_piece(find_index(Piece_Symbols, fen[i]) % 6, Black, board[j]);
            j++;
        }
    }

    return board;
}

int main() {
    struct Square** board = (struct Square**)malloc(EDGE_LEN * EDGE_LEN * sizeof(struct Square*));
    board = create_board(board);

    board = set_board_position(board, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    //board = set_board_position(board, "rnbqkbnr/pppppppp/8/8/2B1P3/5N2/PPPP1PPP/RNBQK2R");
    //board = set_board_position(board, "rnbqkbnr/pppppppp/8/8/3P1B2/2NQ4/PPP1PPPP/R3KBNR");

    while (1) {
        char* src;
        char* dest;

        scanf("%s", src);
        scanf("%s", dest);

        move_piece(board, convert_notation(src), convert_notation(dest));
    
        draw_board(board);

        if (is_move_legal) {
            turn = !turn;
            move++;
        }
    }
    
    return 0;
}
