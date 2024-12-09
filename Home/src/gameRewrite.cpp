#include <stdio.h>
#include <graphics.h>
#include <winbgim.h>

#define KEY_0 48
#define KEY_1 49
#define KEY_2 50
#define MODE_TWO_PLAYERS 0
#define MODE_VS_CPU_RANDOM 1
#define MODE_VS_CPU_STRATEGY 2
#define KEY_ESC 27
#define FOX 1
#define DOGS 2
#define BOARD_SQUARES 8

int GameBoard[BOARD_SQUARES][BOARD_SQUARES] = {0};
int gameMode = -1, playerTurn = 0, winState = 0;
int board_size, board_top, board_left, square_size;
int x,y;
bool redraw = false; //Flag for redrawing the game board

#pragma region Core Functions
void setGameMode(char key) {
    cleardevice();
    char text[200];
    switch (key) {
        case KEY_0:
            sprintf(text,"Mod de joc selectat: Doi Jucatori");
            outtextxy((800-textwidth(text))/2,300,text);
            gameMode=0;
            break;
        case KEY_1:
            sprintf(text,"Eroare - Modul de joc (vs CPU Random) nu este implementat inca");
            outtextxy((800-textwidth(text))/2,300,text);
            gameMode=1;
            break;
        case KEY_2:
            sprintf(text,"Eroare - Modul de joc (vs CPU Strategie) nu este implementat inca");
            outtextxy((800-textwidth(text))/2,300,text);
            gameMode=2;
            break;
        default:
            sprintf(text,"Eroare - Selectie Invalida");
            outtextxy((800-textwidth(text))/2,300,text);
            break;
    }
}

bool gameSetup() {
    bool valid = false;
    char key;
    char text[100];
    sprintf(text, "Select game mode:");
    outtextxy((800 - textwidth(text)) / 2, 200, text);
    sprintf(text, "0 - Two Players     1 - Vs CPU (Random)     2 - Vs CPU (Strategy)");
    outtextxy((800 - textwidth(text)) / 2, 300, text);

    while (!valid) {
        if (kbhit()) {
            key = getch();
            if (key == KEY_ESC) {
                return true; // Signal to return to the main menu
            }
            if (key < 48 || key > 50) {
                sprintf(text, "Error: Invalid selection. Press 0, 1, or 2.");
                outtextxy((800 - textwidth(text)) / 2, 500, text);
            } else {
                valid = true; // Valid input
            }
        }
    }

    setGameMode(key); // Call `setGameMode` to handle the valid key
    return false; // Signal that the setup was successful
}

void resetGameState() {
    // Clear the game board
    for (int i = 0; i < BOARD_SQUARES; i++) {
        for (int j = 0; j < BOARD_SQUARES; j++) {
            GameBoard[i][j] = 0;
        }
    }
    gameMode = -1; // Reset game mode
    playerTurn = FOX; // Reset player turn
    winState=0;
}

void saveGame() {
    setcolor(WHITE);
    char text[100];
    sprintf(text,"Would you like to save the match before exiting?");
    outtextxy((800-textwidth(text))/2,200,text);
    sprintf(text,"0 - NO                    1 - YES");
    outtextxy((800-textwidth(text))/2,300,text);
    while (1) {
        if(kbhit()) {
            char key=getch();
            if(key==KEY_1) {
                FILE *saveFile;
                saveFile = fopen("save.txt", "w");
                fprintf(saveFile,"%d %d\n",gameMode,playerTurn);
                for(int i=0; i<8; i++) {
                    for(int j=0; j<8; j++)
                        fprintf(saveFile,"%d ",GameBoard[i][j]);
                    fprintf(saveFile,"\n");
                }
                fclose(saveFile);
                break;
            }
            if(key==KEY_0) break;
            sprintf(text,"Error - Invalid selection. Press 0 or 1.");
            outtextxy((800-textwidth(text))/2,500,text);
        }
    }
}

void writeTable() {
    for(int i=0; i<8; i++) {
        for(int j=0; j<8; j++)
            printf("%d ",GameBoard[i][j]);
        printf("\n");
    }
}
#pragma endregion

#pragma region Draw Function
void drawSquare(int square_color, int x1, int y1, int x2, int y2) {
    setfillstyle(SOLID_FILL,square_color);
    //Coloram patratul
    bar(x1,y1,x2,y2);
    setcolor(DARKGRAY);
    rectangle(x1,y1,x2,y2);
}

void drawPiece(int player, int x, int y) {
    setfillstyle(SOLID_FILL, player);
    fillellipse(x+square_size/2,y+square_size/2,square_size/4,square_size/4);
    setcolor(WHITE);
    circle(x+square_size/2,y+square_size/2,square_size/4);
}

void highlightPiece(int line, int col) {
    int xc = board_left+col*square_size+square_size/2;
    int yc = board_top+line*square_size+square_size/2;

    // Make a slightly bigger radius than the piece radius for a bold contour
    int highlightRadius = (square_size/4) + 2;

    setcolor(LIGHTGREEN);
    setlinestyle(SOLID_LINE, 0, 3); // Draw a thick line
    circle(xc, yc, highlightRadius);
    setlinestyle(SOLID_LINE, 0, 1);
    setcolor(WHITE);
}

void drawBoard() {
    board_size = 400;
    square_size = board_size / BOARD_SQUARES;
    board_top = (getmaxy() - board_size + 1) / 2;
    board_left = (getmaxx() - board_size + 1) / 2;
    for(int i = 0; i < BOARD_SQUARES; i++)
    for (int j = 0; j < BOARD_SQUARES; j++)
    {
        int color = DARKGRAY;
        if((i+j) %2 == 0) color = BLACK;
        int x1,x2,y1,y2;
        //Coordonate patrat
        x1 = board_left + j * square_size;
        y1 = board_top + i * square_size;
        x2 = x1 + square_size;
        y2 = y1 + square_size;
        //desenare patrat
        drawSquare(color,x1,y1,x2,y2);
        if(GameBoard[i][j] != 0) drawPiece(GameBoard[i][j],x1,y1);
    }
}

void highlightPossibleMoves(int player, int line, int col) {
    int dest_line, dest_col;

    // Iterate over all possible moves
    for (int dx = -1; dx <= 1; dx+=2) {
        for (int dy = -1; dy <= 1; dy+=2) {
            dest_line = line + dx;
            dest_col = col + dy;

            // Check if the move is legal
            if (isLegalMove(player, line, col, dest_line, dest_col)) {
                // Calculate the coordinates of the square
                int x1 = board_left + dest_col * square_size;
                int y1 = board_top + dest_line * square_size;
                int x2 = x1 + square_size;
                int y2 = y1 + square_size;

                // Highlight the square
                int color = LIGHTRED;
                drawSquare(color, x1, y1, x2, y2);
            }
        }
    }
}
#pragma endregion

#pragma region Movement Logic
bool isLegalMove(int player, int start_line, int start_col, int dest_line, int dest_col) {
    // Check if cell is free
    if(GameBoard[dest_line][dest_col]!=0) return false;

    // validate destination boundaries
    if (dest_line < 0 || dest_line >= BOARD_SQUARES || dest_col < 0 || dest_col >= BOARD_SQUARES) {
        return false; 
    }

    // Movement rules:
    if(player == DOGS)
        // Dogs can only move forward diagonally
        if(start_line - dest_line == 1 && abs(start_col - dest_col) == 1) return true;
    if(player == FOX)
        // Fox can move one square diagonally in any direction
        if(abs(start_line - dest_line) == 1 && abs(start_col - dest_col) == 1) return true;
    return false;
}

int movesLeft(int line, int column) {
    int moves = 0;
    // Verifică diagonală sus-stânga
    if (line > 0 && column > 0 && GameBoard[line - 1][column - 1] == 0)    moves++;
    // Verifică diagonală sus-dreapta
    if (line > 0 && column < 7 && GameBoard[line - 1][column + 1] == 0)    moves++;
    // Verifică diagonală jos-stânga
    if (line < 7 && column > 0 && GameBoard[line + 1][column - 1] == 0)    moves++;
    // Verifică diagonală jos-dreapta
    if (line < 7 && column < 7 && GameBoard[line + 1][column + 1] == 0)    moves++;
    return moves;
}

void movePiece() {
    if(ismouseclick(WM_LBUTTONDOWN)) {
        clearmouseclick(WM_LBUTTONDOWN);
        x = mousex(); y = mousey();
        int start_line = (y-board_top)/square_size; int start_col = (x-board_left)/square_size;
        //Selectare piesa
        if(GameBoard[start_line][start_col]==playerTurn) {
            highlightPiece(start_line,start_col);
            highlightPossibleMoves(playerTurn,start_line,start_col);
            while (1) {
                if(ismouseclick(WM_LBUTTONDOWN)) {
                    clearmouseclick(WM_LBUTTONDOWN);
                    x = mousex(); y=mousey();
                    int line = (y-board_top)/square_size; int column = (x-board_left)/square_size;
                    if(isLegalMove(GameBoard[start_line][start_col],start_line,start_col,line,column)) {
                        GameBoard[line][column]=GameBoard[start_line][start_col];
                        GameBoard[start_line][start_col]=0;
                        if(gameMode == MODE_TWO_PLAYERS) {
                            if(playerTurn == FOX) playerTurn = DOGS;
                            else if (playerTurn == DOGS) playerTurn = FOX;
                        }
                    }
                    redraw=true;
                    break;
                }
            }
        }
    }
}
#pragma endregion

#pragma region Computer Logic

#pragma endregion

void checkWin() {
    // Verifică dacă vulpea a ajuns pe ultima linie
    for (int i = 0; i < BOARD_SQUARES; i++) {
        if (GameBoard[BOARD_SQUARES-1][i] == FOX) { // Linia 7
            winState = 1;
            return;
        }
    }
    // Verifică dacă vulpea mai are mutări posibile
    for (int i = 0; i < BOARD_SQUARES; i++)
        for (int j = 0; j < BOARD_SQUARES; j++)
            if (GameBoard[i][j] == FOX) // Găsește poziția vulpii
                if (movesLeft(i,j)) {
                    winState = 0; // Vulpea mai are mutări, jocul continuă
                    return;
                }
    // Dacă nu mai are mutări, câinii au câștigat
    winState = 2;
}

int main() {
    FILE *saveFile;
    initwindow(800, 600, "Proiect");
    char newGame;

    while (1) {
        cleardevice();
        setcolor(WHITE);

        // Display main menu
        char text[100];
        sprintf(text, "Select an option:");
        outtextxy((800 - textwidth(text)) / 2, 200, text);
        sprintf(text, "0 - New Game          1 - Continue Last");
        outtextxy((800 - textwidth(text)) / 2, 300, text);

        // Menu interface with input validation
        while (1) {
            if (kbhit()) {
                newGame = getch();
                if (newGame == KEY_ESC) {
                    closegraph();
                    return 0; // Exit program if ESCAPE is pressed in the main menu
                }
                if (newGame == KEY_0 || newGame == KEY_1) {
                    break; // Valid input
                }
                sprintf(text, "Error: Invalid selection. Press 1 or 0.");
                outtextxy((800 - textwidth(text)) / 2, 500, text);
            }
        }

        cleardevice();

        // Handle main menu selection
        switch (newGame) {
            case KEY_0: // New Game
                resetGameState();
                if (gameSetup()) {
                    // If ESCAPE was pressed during `gameSetup`, return to main menu
                    continue;
                }
                // Setup the initial game board
                for (int i = 0; i < 8; i += 2) GameBoard[7][i] = DOGS;
                GameBoard[0][3] = FOX;
                cleardevice(); drawBoard();
                break;

            case KEY_1: // Continue Last
                saveFile = fopen("save.txt", "r");
                if (saveFile == NULL) {
                    closegraph();
                    printf("Error: Could not open save file.\n");
                    return 1;
                }
                fscanf(saveFile, "%d %d", &gameMode, &playerTurn);
                for (int i = 0; i < 8; i++) {
                    for (int j = 0; j < 8; j++) {
                        fscanf(saveFile, "%d", &GameBoard[i][j]);
                    }
                }
                fclose(saveFile);
                cleardevice(); drawBoard();
                break;

            default:
                printf("Error: Invalid selection.\n");
                return 0;
        }

        // Game Loop
        while (1) {
            if (redraw) {
                cleardevice();
                drawBoard();
                redraw = false; // Prevent unnecessary redraws
            }

            // Check for win condition
            checkWin();

            if(winState==FOX) printf("A castigat vulpea!");
            else if(winState==DOGS) printf("Au castigat cainii!");
            else movePiece();

            if (kbhit()) {
                char key = getch();
                if (key == KEY_ESC) {
                    cleardevice();
                    if(winState == 0) saveGame();
                    break; // Exit game loop
                }
                //Debugging
                //sprintf(text, "You pressed: %c (%d)", key, key);
                //outtextxy((800 - textwidth(text)) / 2, 500, text);
            }

            delay(50);
        }
    }

    closegraph();
    return 0;
}
