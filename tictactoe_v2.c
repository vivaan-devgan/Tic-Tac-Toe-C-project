/*
 * Tic-Tac-Toe v2 (C)
 * New in this version:
 *   - ANSI colours (X = red, O = blue, winning line highlighted)
 *   - Screen refresh after every move
 *   - Box-drawing grid
 *   - Score counter (X wins / O wins / draws)
 *   - Player names
 *   - Welcome banner + menu
 *
 * Computer strategy (rule-based AI):
 *   1. Win if possible   2. Block the player   3. Take centre   4. Random
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ---------- ANSI colour codes ---------- */
#define RED    "\033[1;31m"
#define BLUE   "\033[1;34m"
#define YELLOW "\033[1;33m"
#define GREEN  "\033[1;32m"
#define DIM    "\033[2m"
#define REV    "\033[7m"      /* reverse video = highlight */
#define RESET  "\033[0m"

/* ---------- Global game data ---------- */
char board[3][3];
char nameX[30] = "Player X";
char nameO[30] = "Player O";
int scoreX = 0, scoreO = 0, draws = 0;

/* All 8 winning lines, as cell indices 0-8 */
static const int LINES[8][3] = {
    {0,1,2}, {3,4,5}, {6,7,8},      /* rows */
    {0,3,6}, {1,4,7}, {2,5,8},      /* columns */
    {0,4,8}, {2,4,6}                /* diagonals */
};

/* ---------- Input helpers ---------- */

/* Throw away the rest of the current input line */
void flushLine(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}

/* Read an integer; returns -1 on bad input; quits cleanly on EOF */
int readInt(void) {
    int n;
    int r = scanf("%d", &n);
    if (r == EOF) exit(0);
    flushLine();
    return (r == 1) ? n : -1;
}

/* Read a single non-space character */
char readChar(void) {
    char c;
    if (scanf(" %c", &c) == EOF) exit(0);
    flushLine();
    return c;
}

/* Read a name (spaces allowed, max 29 chars) */
void readName(char *dest) {
    if (scanf(" %29[^\n]", dest) == EOF) exit(0);
    flushLine();
}

/* Wait for Enter */
void pauseEnter(void) {
    printf("\nPress Enter to continue...");
    if (getchar() == EOF) exit(0);
}

/* ---------- Screen helpers ---------- */

void clearScreen(void) {
    printf("\033[2J\033[H");    /* clear screen + cursor to top-left */
}

void printBanner(void) {
    printf(YELLOW);
    printf("╔════════════════════════════╗\n");
    printf("║      TIC - TAC - TOE       ║\n");
    printf("╚════════════════════════════╝\n");
    printf(RESET);
}

void printScoreLine(void) {
    printf(RED "%s (X): %d" RESET "  |  " BLUE "%s (O): %d" RESET
           "  |  Draws: %d\n", nameX, scoreX, nameO, scoreO, draws);
}

/* ---------- Board logic ---------- */

void initBoard(void) {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            board[i][j] = ' ';
}

/* Get the symbol at cell index 0-8 */
char cellAt(int idx) {
    return board[idx / 3][idx % 3];
}

/* pos is 1-9; valid if in range and empty */
int isValidMove(int pos) {
    if (pos < 1 || pos > 9) return 0;
    return cellAt(pos - 1) == ' ';
}

void makeMove(int pos, char sym) {
    board[(pos - 1) / 3][(pos - 1) % 3] = sym;
}

/* Returns index (0-7) of the winning line, or -1 if nobody has won */
int winningLineIndex(void) {
    for (int l = 0; l < 8; l++) {
        char a = cellAt(LINES[l][0]);
        if (a != ' ' && a == cellAt(LINES[l][1]) && a == cellAt(LINES[l][2]))
            return l;
    }
    return -1;
}

/* 'X' or 'O' = winner, 'D' = draw, ' ' = game continues */
char checkWinner(void) {
    int l = winningLineIndex();
    if (l >= 0) return cellAt(LINES[l][0]);

    for (int i = 0; i < 9; i++)
        if (cellAt(i) == ' ') return ' ';
    return 'D';
}

/* Print one cell; highlight if it belongs to the winning line hl */
void printCell(int idx, int hl) {
    char s = cellAt(idx);
    int inLine = 0;

    if (hl >= 0)
        for (int k = 0; k < 3; k++)
            if (LINES[hl][k] == idx) inLine = 1;

    printf(" ");
    if (s == ' ')
        printf(DIM "%d" RESET, idx + 1);          /* faded position number */
    else
        printf("%s%s%c" RESET, inLine ? REV : "", (s == 'X') ? RED : BLUE, s);
    printf(" ");
}

/* Draw the grid. hl = winning line index to highlight, or -1 for none */
void printBoard(int hl) {
    printf("\n    ┌───┬───┬───┐\n");
    for (int r = 0; r < 3; r++) {
        printf("    │");
        for (int c = 0; c < 3; c++) {
            printCell(r * 3 + c, hl);
            printf("│");
        }
        printf("\n");
        if (r < 2) printf("    ├───┼───┼───┤\n");
    }
    printf("    └───┴───┴───┘\n\n");
}

/* ---------- Computer AI ---------- */

/* Return a position (1-9) where sym would win immediately, else 0 */
int findWinningMove(char sym) {
    for (int pos = 1; pos <= 9; pos++) {
        if (isValidMove(pos)) {
            makeMove(pos, sym);                          /* try */
            char w = checkWinner();
            board[(pos - 1) / 3][(pos - 1) % 3] = ' ';   /* undo */
            if (w == sym) return pos;
        }
    }
    return 0;
}

/* Computer plays 'O', human plays 'X' */
int computerMove(void) {
    int pos;

    pos = findWinningMove('O');        /* 1. win */
    if (pos) return pos;

    pos = findWinningMove('X');        /* 2. block */
    if (pos) return pos;

    if (isValidMove(5)) return 5;      /* 3. centre */

    do {                               /* 4. random */
        pos = rand() % 9 + 1;
    } while (!isValidMove(pos));
    return pos;
}

/* ---------- One full game ---------- */

void playGame(int vsComputer) {
    char current = 'X';
    char result;
    char msg[80] = "";

    initBoard();

    while (1) {
        int pos;

        if (vsComputer && current == 'O') {
            /* computer's turn: no screen redraw, just move */
            pos = computerMove();
            snprintf(msg, sizeof msg, "Computer chose position %d", pos);
        } else {
            /* human's turn: redraw the screen */
            clearScreen();
            printBanner();
            printScoreLine();
            printBoard(-1);
            if (msg[0]) {
                printf(GREEN "%s\n" RESET, msg);
                msg[0] = '\0';
            }
            printf("%s%s" RESET " (%c), enter position (1-9): ",
                   (current == 'X') ? RED : BLUE,
                   (current == 'X') ? nameX : nameO, current);
            pos = readInt();

            if (!isValidMove(pos)) {
                snprintf(msg, sizeof msg, "Invalid or taken! Try again.");
                continue;              /* redraw and ask again */
            }
        }

        makeMove(pos, current);

        result = checkWinner();
        if (result != ' ') break;

        current = (current == 'X') ? 'O' : 'X';
    }

    /* update scores */
    if (result == 'X')      scoreX++;
    else if (result == 'O') scoreO++;
    else                    draws++;

    /* final screen */
    clearScreen();
    printBanner();
    printScoreLine();
    printBoard(winningLineIndex());

    if (msg[0]) printf(GREEN "%s\n" RESET, msg);

    if (result == 'D')
        printf(YELLOW "It's a draw!\n" RESET);
    else if (result == 'X')
        printf(RED "%s wins! Congratulations!\n" RESET, nameX);
    else if (vsComputer)
        printf(BLUE "Computer wins! Better luck next time.\n" RESET);
    else
        printf(BLUE "%s wins! Congratulations!\n" RESET, nameO);
}

/* ---------- Menu pieces ---------- */

void showScores(void) {
    clearScreen();
    printBanner();
    printf("\n--- SCOREBOARD ---\n");
    printf(RED  "%s (X) wins : %d\n" RESET, nameX, scoreX);
    printf(BLUE "%s (O) wins : %d\n" RESET, nameO, scoreO);
    printf("Draws          : %d\n", draws);
    printf("Total games    : %d\n", scoreX + scoreO + draws);
    pauseEnter();
}

void askNames(int vsComputer) {
    printf("\nEnter name for Player X: ");
    readName(nameX);
    if (vsComputer) {
        strcpy(nameO, "Computer");
    } else {
        printf("Enter name for Player O: ");
        readName(nameO);
    }
}

void playMatch(void) {
    static int currentMode = 0;      /* remembers last mode so names/scores persist */
    int mode;
    char again;

    printf("\n1. Two Player\n2. Vs Computer\nChoose mode: ");
    mode = readInt();
    if (mode != 1 && mode != 2) {
        printf("Please choose 1 or 2.\n");
        pauseEnter();
        return;
    }

    if (mode != currentMode) {       /* new mode -> new names, fresh scores */
        askNames(mode == 2);
        scoreX = scoreO = draws = 0;
        currentMode = mode;
    }

    do {
        playGame(mode == 2);
        printf("\nPlay again? (y/n): ");
        again = readChar();
    } while (again == 'y' || again == 'Y');
}

/* ---------- main ---------- */

int main(void) {
    srand((unsigned)time(NULL));
    int choice;

    do {
        clearScreen();
        printBanner();
        printf("\n  1. Play\n  2. View Scores\n  3. Exit\n\nChoose: ");
        choice = readInt();

        switch (choice) {
            case 1: playMatch();  break;
            case 2: showScores(); break;
            case 3: break;
            default:
                printf("Please choose 1, 2 or 3.\n");
                pauseEnter();
        }
    } while (choice != 3);

    clearScreen();
    printf("Thanks for playing!\n");
    return 0;
}
