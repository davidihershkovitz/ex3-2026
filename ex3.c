#include <stdio.h>

#ifndef ROWS
#define ROWS 6
#endif

#ifndef COLS
#define COLS 7
#endif

#define CONNECT_N 4

/* Tokens */
#define EMPTY '.'
#define TOKEN_P1 'X'
#define TOKEN_P2 'O'

#define HUMAN 1
#define COMPUTER 2

int isColumnFull(char board[][COLS], int col);

int isBoardFull(char board[][COLS], int cols);

/* Return index of row where token will land, or -1 if column full */
int getFreeRow(char board[][COLS], int rows, int col);

/* Place token in column (0-based). Return row index or -1 if illegal */
int makeMove(char board[][COLS], int rows, int col, char token);

int checkVictory(char board[][COLS], int rows, int cols, int row, int col, char token);

/* Human player: asks repeatedly until a valid non-full column is chosen (0-based) */
int humanChoose(char board[][COLS], int cols);

/* Computer*/
int computerChoose(char board[][COLS], int rows, int cols, char myToken, char oppToken);

void runConnectFour(char board[][COLS], int rows, int cols, int p1Type, int p2Type);

void initBoard(char board[][COLS], int rows, int cols);

void printBoard(char board[][COLS], int rows, int cols);

int getPlayerType(int playerNumber);

void columnOrder(int order[], int cols);

int checkThree(char board[][COLS], int rows, int cols,int row, int col, char token);



int main() {
    char board[ROWS][COLS];
    printf("Connect Four (%d rows x %d cols)\n\n", ROWS, COLS);
    int p1Type = getPlayerType(1);
    int p2Type = getPlayerType(2);
    initBoard(board, ROWS, COLS);
    printBoard(board, ROWS, COLS);
    runConnectFour(board, ROWS, COLS, p1Type, p2Type);
    return 0;
}

void initBoard(char board[][COLS], int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            board[i][j] = EMPTY;
        }
    }
}

void printBoard(char board[][COLS], int rows, int cols) {
    printf("\n");
    for (int r = 0; r < rows; r++) {
        printf("|");
        for (int c = 0; c < cols; c++) {
            putchar(board[r][c]);
            printf("|");
        }
        printf("\n");
    }
    for (int c = 1; c <= cols; c++) {
        printf(" %d", c % 10);
    }
    printf("\n\n");
}

int getPlayerType(int playerNumber) {
    char ch;
    while (1) {
        printf("Choose type for player %d: h - human, c - computer: ", playerNumber);
        int n = scanf(" %c", &ch);
        if (n != 1) {
            printf("Input error. Try again.\n");
            while (getchar() != '\n'); // clear input buffer
            continue;
        }
        if (ch == 'h' || ch == 'H') return HUMAN;
        if (ch == 'c' || ch == 'C') return COMPUTER;

        printf("Invalid selection. Enter h or c.\n");
        while (getchar() != '\n'); // clear rest of input
    }
}

//get the row for the token placement
int getFreeRow(char board[][COLS], int rows, int col) {
    for (int i = rows - 1; i >= 0; i--) {
        if (board[i][col] == EMPTY) {
            return i;
        }
    }
    return -1;
}
//place a token in a valid place
int makeMove(char board[][COLS], int rows, int col, char token) {
    int row = getFreeRow(board, rows, col);
    if(row == -1) return -1;
    board[row][col] = token;
    return row;
}

//check if the first place in x col is empty or not
int isColumnFull(char board[][COLS], int col) {
    return board[0][col] != EMPTY;
}

//simply running a quick check to see if there's any col left for placement
int isBoardFull(char board[][COLS], int cols) {
    for (int c = 0; c < cols; c++) {
        if (!isColumnFull(board, c)) {
            return 0;
        }
    }
    return 1; // all the cols are full
}

int humanChoose(char board[][COLS], int cols) {
    int col;
    int new;

    while (1) {
        printf("Enter column (1-%d): ", cols);

        // try to get a num
        if (scanf("%d", &col) != 1) {
            // not a num
            printf("Invalid input. Enter a number.\n");

            // cleaning the remaining chars
            while ((new = getchar()) != '\n' && new != EOF);
            continue;
        }

        // cleaning the buffer
        while ((new = getchar()) != '\n' && new != EOF);

        // outside of range
        if (col < 1 || col > cols) {
            printf("Invalid column. Choose between 1 and %d.\n", cols);
            continue;
        }

        // col is full
        if (isColumnFull(board, col - 1)) {
            printf("Column %d is full. Choose another column.\n", col);
            continue;
        }

        return col - 1;
    }
}
//ordering the cols by cases (odd/even)
void columnOrder(int order[], int cols) {
    int midLeft, midRight;
    int idx = 0;

    if (cols % 2 == 1) {
        // odd number of columns
        int mid = cols / 2;
        order[idx++] = mid;

        for (int i = 1; i <= mid; i++) {
            if (mid - i >= 0)
                order[idx++] = mid - i;
            if (mid + i < cols)
                order[idx++] = mid + i;
        }
    } else {
        // even number of columns
        midLeft  = cols/2 - 1;
        midRight = cols/2;

        order[idx++] = midLeft;
        order[idx++] = midRight;

        int j = 1;
        while (idx < cols) {
            if (midLeft - j >= 0)
                order[idx++] = midLeft - j;
            if (midRight + j < cols)
                order[idx++] = midRight + j;
            j++;
        }
    }
}
int computerChoose(char board[][COLS], int rows, int cols, char myToken, char humanToken) {
    int order[COLS];
    columnOrder(order, cols);   // order the cols according to instruction

    int freeRow;

    // case 1- we check via checkVictory if our next move is a win move
    for (int i = 0; i < cols; i++) {
        int col = order[i];

        if (isColumnFull(board, col))
            continue;

        freeRow = getFreeRow(board, rows, col);

        board[freeRow][col] = myToken;
        int win = checkVictory(board, rows, cols, freeRow, col, myToken);
        board[freeRow][col] = EMPTY;

        if (win)
            return col;
    }

    // case 2- exactly like case 1 but we check for the opp
    for (int i = 0; i < cols; i++) {
        int col = order[i];

        if (isColumnFull(board, col))
            continue;

        freeRow = getFreeRow(board, rows, col);

        board[freeRow][col] = humanToken;
        int oppWin = checkVictory(board, rows, cols, freeRow, col, humanToken);
        board[freeRow][col] = EMPTY;

        if (oppWin)
            return col;
    }

    // case 3 - we use a func like checkVictory to see if there's a 3 seq
    for (int i = 0; i < cols; i++) {
        int col = order[i];

        if (isColumnFull(board, col))
            continue;

        freeRow = getFreeRow(board, rows, col);

        board[freeRow][col] = myToken;
        int three = checkThree(board, rows, cols, freeRow, col, myToken);
        board[freeRow][col] = EMPTY;

        if (three >= 3)
            return col;
    }

    // case 4- exactly like case 3 but we check for the opp
    for (int i = 0; i < cols; i++) {
        int col = order[i];

        if (isColumnFull(board, col))
            continue;

        freeRow = getFreeRow(board, rows, col);

        board[freeRow][col] = humanToken;
        int three = checkThree(board, rows, cols, freeRow, col, humanToken);
        board[freeRow][col] = EMPTY;

        if (three >= 3)
            return col;
    }

    // case 5 - choose the first optimal col
    for (int i = 0; i < cols; i++) {
        int col = order[i];
        if (!isColumnFull(board, col))
            return col;
    }

    return 0; // we won't return from here but for the sport haha
}
int checkThree(char board[][COLS], int rows, int cols,int row, int col, char token) {
    int r, c, count, max = 1;

    // horizontal
    count = 1;
    c = col + 1; while (c < cols && board[row][c] == token) { count++; c++; }
    c = col - 1; while (c >= 0   && board[row][c] == token) { count++; c--; }
    if (count > max) max = count;

    // vertical
    count = 1;
    r = row + 1; while (r < rows && board[r][col] == token) { count++; r++; }
    r = row - 1; while (r >= 0   && board[r][col] == token) { count++; r--; }
    if (count > max) max = count;

    // main-diag
    count = 1;
    r = row + 1; c = col + 1; while (r < rows && c < cols && board[r][c] == token) { count++; r++; c++; }
    r = row - 1; c = col - 1; while (r >= 0   && c >= 0   && board[r][c] == token) { count++; r--; c--; }
    if (count > max) max = count;

    // anti-diag
    count = 1;
    r = row - 1; c = col + 1; while (r >= 0   && c < cols && board[r][c] == token) { count++; r--; c++; }
    r = row + 1; c = col - 1; while (r < rows && c >= 0   && board[r][c] == token) { count++; r++; c--; }
    if (count > max) max = count;

    return max;
}

int checkVictory(char board[][COLS], int rows, int cols, int row, int col, char token)
{
    int r, c;
    int count;

    // horizontal
    count = 1;

    // right
    c = col + 1;
    while (c < cols && board[row][c] == token) { count++; c++; }

    // left
    c = col - 1;
    while (c >= 0 && board[row][c] == token) { count++; c--; }

    if (count >= CONNECT_N) return 1;

    // vertical
    count = 1;

    // down
    r = row + 1;
    while (r < rows && board[r][col] == token) { count++; r++; }

    // up
    r = row - 1;
    while (r >= 0 && board[r][col] == token) { count++; r--; }

    if (count >= CONNECT_N) return 1;

    // main diag
    count = 1;

    // down
    r = row + 1;  c = col + 1;
    while (r < rows && c < cols && board[r][c] == token) { count++; r++; c++; }

    // up
    r = row - 1;  c = col - 1;
    while (r >= 0 && c >= 0 && board[r][c] == token) { count++; r--; c--; }

    if (count >= CONNECT_N) return 1;

    // anti-diag
    count = 1;

    // up
    r = row - 1;  c = col + 1;
    while (r >= 0 && c < cols && board[r][c] == token) { count++; r--; c++; }

    // down
    r = row + 1;  c = col - 1;
    while (r < rows && c >= 0 && board[r][c] == token) { count++; r++; c--; }

    if (count >= CONNECT_N) return 1;

    // no victory
    return 0;
}

void runConnectFour(char board[][COLS], int rows, int cols, int p1Type, int p2Type) {
    int currentPlayer = 1;
    int col, row;

    while (1) {

        char token = (currentPlayer == 1 ? TOKEN_P1 : TOKEN_P2);
        char oppToken = (currentPlayer == 1 ? TOKEN_P2 : TOKEN_P1);

        printf("Player %d (%c) turn.\n", currentPlayer, token);

        // depends on the player type
        if (currentPlayer == 1) {
            if (p1Type == HUMAN) {
                col = humanChoose(board, cols);
            } else {
                col = computerChoose(board, rows, cols, token, oppToken);
                printf("Computer chose column %d\n", col + 1);
            }
        } else { // player 2
            if (p2Type == HUMAN) {
                col = humanChoose(board, cols);
            } else {
                col = computerChoose(board, rows, cols, token, oppToken);
                printf("Computer chose column %d\n", col + 1);
            }
        }

        // make a move
        row = makeMove(board, rows, col, token);

        // print every time
        printBoard(board, rows, cols);

        // check if there's a victory
        if (checkVictory(board, rows, cols, row, col, token)) {
            printf("Player %d (%c) wins!\n", currentPlayer, token);
            return;
        }

        // check if there's a tie
        if (isBoardFull(board, cols)) {
            printf("Board full and no winner. It's a tie!\n");
            return;
        }

        // switch players
        currentPlayer = (currentPlayer == 1 ? 2 : 1);
    }
}
