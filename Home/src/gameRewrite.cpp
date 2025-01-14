#include <stdio.h>
#include <graphics.h>
#include <winbgim.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

//input
#define KEY_0 48
#define KEY_1 49
#define KEY_2 50
#define KEY_ESC 27
//game logic
#define MODE_TWO_PLAYERS 0
#define MODE_VS_CPU_RANDOM 1
#define MODE_VS_CPU_STRATEGY 2
#define FOX 1
#define DOGS 2
#define BOARD_SQUARES 8

//screen
int screen_width = 800;
int screen_height = 600;
//game logic
int gameMode = -1, playerTurn = 0, winState = 0, strategy = 0, advanceOrder = 0;
bool computerTurn = false; //flag for computer turn
bool advance = false; //flag for dog advance
//game board
int board_size, board_top, board_left, square_size;
int GameBoard[BOARD_SQUARES][BOARD_SQUARES] = {0};
//mouse
int x,y;

bool redraw = false; //Flag for redrawing the game board

#pragma region Core Functions
void setGameMode(char key) {
    cleardevice();
    char text[200];
    switch (key) {
        case KEY_0:
            printf("Mod de joc selectat: Doi Jucatori\n");
            gameMode=MODE_TWO_PLAYERS;
            break;
        case KEY_1:
            printf("Mod de joc selectat: Vs CPU (Random)\n");
            gameMode=MODE_VS_CPU_RANDOM;
            break;
        case KEY_2:
            printf("Mod de joc selectat: Vs CPU (Strategie)\n");
            gameMode=MODE_VS_CPU_STRATEGY;
            break;
        default:
            printf("Eroare - Selectie Invalida\n");
            break;
    }
}

bool gameSetup() {
    bool valid = false;
    char key;
    char text[100];
    sprintf(text, "Select game mode:");
    outtextxy((screen_width - textwidth(text)) / 2, 200, text);
    sprintf(text, "0 - Two Players     1 - Vs CPU (Random)     2 - Vs CPU (Strategy)");
    outtextxy((screen_width - textwidth(text)) / 2, screen_height/2, text);

    while (!valid) {
        if (kbhit()) {
            key = getch();
            if (key == KEY_ESC) {
                return true; // Signal to return to the main menu
            }
            if (key < KEY_0 || key > KEY_2) {
                sprintf(text, "Error: Invalid selection. Press 0, 1, or 2.");
                outtextxy((screen_width - textwidth(text)) / 2, 500, text);
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
    advance=false;
    strategy = 0;
    computerTurn = false;
    advanceOrder = 0;
}

void saveGame() {
    setcolor(WHITE);
    char text[100];
    sprintf(text,"Would you like to save the match before exiting?");
    outtextxy((screen_width-textwidth(text))/2,200,text);
    sprintf(text,"0 - NO                    1 - YES");
    outtextxy((screen_width-textwidth(text))/2,screen_height/2,text);
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
            outtextxy((screen_width-textwidth(text))/2,500,text);
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
    int color = FOX;
    if(player>1) {
        char text[10];
        sprintf(text, "%d", player-2);
        setcolor(WHITE);
        outtextxy(x+square_size-textwidth(text),y+square_size-textheight(text),text);
        color = DOGS;
    }
    setfillstyle(SOLID_FILL, color);
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
    //draw positions
    char letter = 'a';
    char nr = '8';
    char text[10];
    for(int s = 0; s < BOARD_SQUARES; s++) {
        sprintf(text,"%c",letter+s);
        outtextxy(board_left+square_size*s+(square_size-textwidth(text))/2,board_size+board_top+(square_size-textheight(text))/2,text);
        sprintf(text,"%c",nr-s);
        outtextxy(board_left-(square_size-textwidth(text))/2,board_top+square_size*s+(square_size-textheight(text))/2,text);
    }
}
#pragma endregion

void showTurn() {
    setcolor(WHITE);
    char text[50];
    sprintf(text, "It's Player %d's Turn", playerTurn);
    outtextxy((screen_width-textwidth(text)) / 2, 50, text);
}

#pragma region Movement Logic

bool isLegalMove(int player, int start_line, int start_col, int dest_line, int dest_col) {
    // Check if cell is free
    if(GameBoard[dest_line][dest_col]!=0) return false;

    // validate destination boundaries
    if (dest_line < 0 || dest_line >= BOARD_SQUARES || dest_col < 0 || dest_col >= BOARD_SQUARES) {
        return false; 
    }

    // Movement rules:
    if(player > FOX)
        // Dogs can only move forward diagonally
        if(start_line - dest_line == 1 && abs(start_col - dest_col) == 1) return true;
    if(player == FOX)
        // Fox can move one square diagonally in any direction
        if(abs(start_line - dest_line) == 1 && abs(start_col - dest_col) == 1) return true;
    return false;
}

void highlightPossibleMoves(int line, int col) {
    //declare necessary variables
    int dest_line, dest_col;
    int player = GameBoard[line][col];

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

void unHighlight(int old_line, int old_col) {
    //declare necessary variables
    int dest_line, dest_col;
    int player = GameBoard[old_line][old_col];
    int x1, y1, x2, y2;
    int color = DARKGRAY;

    //coordinates for piece
    x1 = board_left + old_col * square_size;
    y1 = board_top + old_line * square_size;
    x2 = x1 + square_size;
    y2 = y1 + square_size;

    drawSquare(color,x1,y1,x2,y2);
    drawPiece(player,x1,y1);

    // Iterate over ALL possible moves (so innefficient)
    for (int dx = -1; dx <= 1; dx+=2) {
        for (int dy = -1; dy <= 1; dy+=2) {
            dest_line = old_line + dx;
            dest_col = old_col + dy;

            // Check if the move is legal ;) let me cry let me cry
            if (isLegalMove(player, old_line, old_col, dest_line, dest_col)) {
                // Calculate the coordinates of the square
                x1 = board_left + dest_col * square_size;
                y1 = board_top + dest_line * square_size;
                x2 = x1 + square_size;
                y2 = y1 + square_size;

                // unhighlight the square bruh
                drawSquare(color, x1, y1, x2, y2);
            }
        }
    }
}

int movesLeft(int line, int column) {
    int moves = 0; 
    int piece = GameBoard[line][column];
    if(piece==FOX) {
        // Verifică diagonală sus-stânga
        if (line > 0 && column > 0 && GameBoard[line - 1][column - 1] == 0)    moves++;
        // Verifică diagonală sus-dreapta
        if (line > 0 && column < 7 && GameBoard[line - 1][column + 1] == 0)    moves++;
        // Verifică diagonală jos-stânga
        if (line < 7 && column > 0 && GameBoard[line + 1][column - 1] == 0)    moves++;
        // Verifică diagonală jos-dreapta
        if (line < 7 && column < 7 && GameBoard[line + 1][column + 1] == 0)    moves++;
    }
    else if(piece>=DOGS) {
        // Verifică diagonală sus-stânga
        if (line > 0 && column > 0 && GameBoard[line - 1][column - 1] == 0)    moves++;
        // Verifică diagonală sus-dreapta
        if (line > 0 && column < 7 && GameBoard[line - 1][column + 1] == 0)    moves++;
    }
    return moves;
}

void movePiece(int p_line, int p_col, int d_line, int d_col) {
    //declare necessary variables
    int player = GameBoard[p_line][p_col];
    int x1, y1, x2, y2, d_x, d_y;

    //move piece
    GameBoard[d_line][d_col] = player;
    GameBoard[p_line][p_col] = 0;

    //calculate old square coords for erasing
    x1 = board_left + p_col * square_size;
    y1 = board_top + p_line * square_size;
    x2 = x1 + square_size;
    y2 = y1 + square_size;

    drawSquare(DARKGRAY, x1, y1, x2, y2);

    //calculate new square coords for drawing piece
    d_x = board_left + d_col * square_size;
    d_y = board_top + d_line * square_size;

    drawPiece(player, d_x, d_y);
    
}

void playerMove() {
    if(ismouseclick(WM_LBUTTONDOWN)) {
        clearmouseclick(WM_LBUTTONDOWN);
        x = mousex(); y = mousey();
        int start_line = (y-board_top)/square_size; int start_col = (x-board_left)/square_size;
        //Selectare piesa
        if((playerTurn==FOX&&GameBoard[start_line][start_col]==playerTurn)||(playerTurn==DOGS&&GameBoard[start_line][start_col]>=playerTurn)) {
            highlightPiece(start_line, start_col);
            highlightPossibleMoves(start_line, start_col);
            while (1) {
                if(ismouseclick(WM_LBUTTONDOWN)) {
                    clearmouseclick(WM_LBUTTONDOWN);
                    x = mousex(); y=mousey();
                    int line = (y-board_top)/square_size; int column = (x-board_left)/square_size;
                    
                    //unHighlight necessary stuff
                    unHighlight(start_line, start_col);

                    if(isLegalMove(GameBoard[start_line][start_col],start_line,start_col,line,column)) {
                        //move piece
                        movePiece(start_line, start_col, line, column);

                        //change Turn Logic
                        if(gameMode == MODE_TWO_PLAYERS) {
                            if(playerTurn == FOX) playerTurn = DOGS;
                            else if (playerTurn == DOGS) playerTurn = FOX;
                        }
                        if(gameMode == MODE_VS_CPU_RANDOM || gameMode == MODE_VS_CPU_STRATEGY) {
                            computerTurn = true;
                        }
                    }
                    break;
                }
            }
        }
    }
}
#pragma endregion

#pragma region Computer Logic

void randomMove() {
    srand(time(NULL));
    int computerPiece; int line, col, possibleMoves=0, move;

    while(!possibleMoves) {
        if (playerTurn==FOX) {
            computerPiece = 2 + rand() % 4;
        }
        if (playerTurn==DOGS) {
            computerPiece = FOX;
        }

        for(int i = 0; i < BOARD_SQUARES; i++)
            for(int j = 0; j < BOARD_SQUARES; j++)
                if(GameBoard[i][j]==computerPiece) {
                    line = i;
                    col = j;
                }

        possibleMoves=movesLeft(line,col);
    }

    int dx, dy, d_line, d_col;
    
    while (1) {
        move = rand()%4;
        switch (move) {
            case 0:
                //stanga sus
                dy = -1; dx = -1;
                break;
            case 1:
                //dreapta sus
                dy = -1; dx = +1;
                break;
            case 2:
                //stanga jos
                dy = +1; dx = -1;
                break;
            case 3:
                //dreapta jos
                dy = +1; dx = +1;
                break;
        
            default:
                break;
    }
        d_line = line + dy;
        d_col = col + dx;
        if(isLegalMove(computerPiece,line,col,d_line,d_col)) {
            movePiece(line,col,d_line,d_col);
            computerTurn = false;
            redraw = true;
            break;
        }
    }
}

void advanceDogs(int order, int *cursor_line, int *cursor_col) {
    int player = GameBoard[*cursor_line][*cursor_col];
    /*int dog_line = -1, dog_col = -1;

    // Find position of the current dog
    for (int i = 0; i < BOARD_SQUARES; i++) {
        for (int j = 0; j < BOARD_SQUARES; j++) {
            if (GameBoard[i][j] == DOGS + *index) {
                dog_line = i;
                dog_col = j;
                break;
            }
        }
    }*/

    /*// Ensure the dog was found
    if (dog_line == -1 || dog_col == -1) {
        printf("Error: Dog with index %d not found on the board.\n", *index);
        return;
    }*/

    // Move the dog based on the order
    if (order == 1) { // Left to right
        if(isLegalMove(player,*cursor_line, *cursor_col, *cursor_line - 1, *cursor_col - 1)) {
            movePiece(*cursor_line, *cursor_col, *cursor_line - 1, *cursor_col - 1);
            //printf("Dog %d successfully advanced.\n", *index+1);
            (*cursor_col)-=2; // Decrement dog index
        }
        else movePiece(*cursor_line+1,*cursor_col+1,*cursor_line-1,*cursor_col-1);
    } else { // Right to left
        if(isLegalMove(player,*cursor_line, *cursor_col, *cursor_line - 1, *cursor_col + 1)) {
            movePiece(*cursor_line, *cursor_col, *cursor_line - 1, *cursor_col + 1);
            //printf("Dog %d successfully advanced.\n", *index+1);
            (*cursor_col)+=2; // Increment dog index
        }
        else movePiece(*cursor_line+1,*cursor_col-1,*cursor_line-1,*cursor_col+1);
    }
    computerTurn = false;

    // Check if all dogs have advanced
    if ((order == 1 && *cursor_col < 1) || (order == 0 && *cursor_col > 6)) {
        advance = false;
        printf("All dogs have advanced by a line.\n");
        if(order==0)highlightPiece(*cursor_line-1,*cursor_col-1);
        if(order==1) highlightPiece(*cursor_line-1,*cursor_col+1);
    }
    else highlightPiece(*cursor_line,*cursor_col);
}

void strategy_one(int counter, int *index) {
    int dog_line = -1, dog_col = -1;
    // Find position of the current dog
    for (int i = 0; i < BOARD_SQUARES; i++) {
        for (int j = 0; j < BOARD_SQUARES; j++) {
            if (GameBoard[i][j] == DOGS + *index) {
                dog_line = i;
                dog_col = j;
                break;
            }
        }
    }

    switch (counter) {
        case 1:
            movePiece(dog_line,dog_col,dog_line-1,dog_col+1);
            printf("Dog %d succesfully moved",*index+1);
            *index = 3;
            printf("Next to move is Dog %d", *index+1);
            break;//correct
        case 2:
            movePiece(dog_line,dog_col,dog_line-1,dog_col-1);
            printf("Dog %d succesfully moved",*index+1);
            *index = 1;
            printf("Next to move is Dog %d", *index+1);
            break;//corect
        case 3:
            movePiece(dog_line,dog_col,dog_line-1,dog_col-1);
            printf("Dog %d succesfully moved",*index+1);
            *index = 2;
            printf("Next to move is Dog %d", *index+1);
            break;//correct
        case 4:
            movePiece(dog_line,dog_col,dog_line-1,dog_col-1);
            printf("Dog %d succesfully moved",*index+1);
            *index = 0;
            printf("Next to move is Dog %d", *index+1);
            break;//correct
        case 5:
            movePiece(dog_line,dog_col,dog_line-1,dog_col+1);
            printf("Dog %d succesfully moved",*index+1);
            *index = 0;
            printf("Next to move is Dog %d", *index+1);
            break;//correct
        case 6:
            movePiece(dog_line,dog_col,dog_line-1,dog_col-1);
            printf("Dog %d succesfully moved",*index+1);
            *index = 2;//correct
            printf("Next to move is Dog %d", *index+1);
            break;
        case 7:
            movePiece(dog_line,dog_col,dog_line-1,dog_col+1);
            printf("Dog %d succesfully moved",*index+1);
            *index = 3;//correct
            printf("Next to move is Dog %d", *index+1);
            break;
        case 8:
            movePiece(dog_line,dog_col,dog_line-1,dog_col+1);
            printf("Dog %d succesfully moved",*index+1);
            printf("All dogs have been moved using the strategy\n");
            strategy = -1;
            break;
        // need to implement a branching position based on the move the fox makes and the current counter
        default:
            break;
    }

    computerTurn = false;
}

void strategy_two(int counter, int *index) {
    int dog_line = -1, dog_col = -1;
    // Find position of the current dog
    for (int i = 0; i < BOARD_SQUARES; i++) {
        for (int j = 0; j < BOARD_SQUARES; j++) {
            if (GameBoard[i][j] == DOGS + *index) {
                dog_line = i;
                dog_col = j;
                break;
            }
        }
    }

    switch (counter) {
        case 1:
            movePiece(dog_line,dog_col,dog_line-1,dog_col-1);
            printf("Dog %d succesfully moved",*index+1);
            *index = 1;
            printf("Next to move is Dog %d", *index+1);
            break;//correct
        case 2:
            movePiece(dog_line,dog_col,dog_line-1,dog_col+1);
            printf("Dog %d succesfully moved",*index+1);
            *index = 3;
            printf("Next to move is Dog %d", *index+1);
            break;//corect
        case 3:
            movePiece(dog_line,dog_col,dog_line-1,dog_col+1);
            printf("Dog %d succesfully moved",*index+1);
            *index = 2;
            printf("Next to move is Dog %d", *index+1);
            break;//correct
        case 4:
            movePiece(dog_line,dog_col,dog_line-1,dog_col+1);
            printf("Dog %d succesfully moved",*index+1);
            printf("All dogs have been moved using the strategy\n");
            strategy = -1;
            advanceOrder=1;
            advance=false;
            break;
        
        default:
            break;
    }

    computerTurn = false;
}

void strategy_three(int counter, int *index) {
    int dog_line = -1, dog_col = -1;
    // Find position of the current dog
    for (int i = 0; i < BOARD_SQUARES; i++) {
        for (int j = 0; j < BOARD_SQUARES; j++) {
            if (GameBoard[i][j] == DOGS + *index) {
                dog_line = i;
                dog_col = j;
                break;
            }
        }
    }

    switch (counter) {
        case 1:
            movePiece(dog_line,dog_col,dog_line-1,dog_col-1);
            printf("Dog %d succesfully moved",*index+1);
            *index = 0;
            printf("Next to move is Dog %d", *index+1);
            break;//correct
        case 2:
            movePiece(dog_line,dog_col,dog_line-1,dog_col+1);
            printf("Dog %d succesfully moved",*index+1);
            *index = 0;
            printf("Next to move is Dog %d", *index+1);
            break;//correct
        case 3:
            movePiece(dog_line,dog_col,dog_line-1,dog_col+1);
            printf("Dog %d succesfully moved",*index+1);
            *index = 1;
            printf("Next to move is Dog %d", *index+1);
            break;//correct
        case 4:
            movePiece(dog_line,dog_col,dog_line-1,dog_col-1);
            printf("Dog %d succesfully moved",*index+1);
            *index = 1;
            printf("Next to move is Dog %d", *index+1);
            break;//correct
        case 5:
            movePiece(dog_line,dog_col,dog_line-1,dog_col-1);
            printf("Dog %d succesfully moved",*index+1);
            *index = 3;
            printf("Next to move is Dog %d", *index+1);
            break;//correct
        case 6:
            movePiece(dog_line,dog_col,dog_line-1,dog_col-1);
            printf("Dog %d succesfully moved",*index+1);
            *index = 2;
            printf("Next to move is Dog %d", *index+1);
            break;//correct
        case 7:
            movePiece(dog_line,dog_col,dog_line-1,dog_col+1);
            printf("Dog %d succesfully moved",*index+1);
            *index = 2;
            printf("Next to move is Dog %d", *index+1);
            break;//correct
        case 8:
            movePiece(dog_line,dog_col,dog_line-1,dog_col+1);
            printf("Dog %d succesfully moved",*index+1);
            printf("All dogs have been moved using the strategy\n");
            strategy = -1;
            advanceOrder=1;
            advance=false;
            *index=0;
            break;
        case 12:
            movePiece(dog_line,dog_col,dog_line-1,dog_col-1);
            printf("Dog %d succesfully moved",*index+1);
            *index = 0;
            printf("Next to move is Dog %d", *index+1);
            break;//correct
        case 13:
            movePiece(dog_line,dog_col,dog_line-1,dog_col-1);
            printf("Dog %d succesfully moved",*index+1);
            *index = 0;
            printf("Next to move is Dog %d", *index+1);
            break;//correct
        case 14:
            movePiece(dog_line,dog_col,dog_line-1,dog_col+1);
            printf("Dog %d succesfully moved",*index+1);
            *index = 1;
            printf("Next to move is Dog %d", *index+1);
            break;//correct
        case 15:
            movePiece(dog_line,dog_col,dog_line-1,dog_col+1);
            printf("Dog %d succesfully moved",*index+1);
            *index = 3;
            printf("Next to move is Dog %d", *index+1);
            break;//correct
        case 16:
            movePiece(dog_line,dog_col,dog_line-1,dog_col+1);
            printf("Dog %d succesfully moved",*index+1);
            *index = 2;
            printf("Next to move is Dog %d", *index+1);
            break;//correct
        case 17:
            movePiece(dog_line,dog_col,dog_line-1,dog_col+1);
            printf("Dog %d succesfully moved",*index+1);
            printf("All dogs have been moved using the strategy\n");
            strategy = -1;
            advanceOrder=1;
            advance=false;
            *index = 2;
            break;//correct
        
        default:
            break;
    }

    computerTurn = false;
}

void mainStrategy() {
    int fox_line, fox_col;
    static int dogIndex, currentMove;
    if(strategy<=0) currentMove = 0;
    // Find fox position and check for strategy case
    for (int i = 0; i < BOARD_SQUARES; i++) 
        for (int j = 0; j < BOARD_SQUARES; j++) 
            if (GameBoard[i][j] == FOX) {
                fox_line = i;
                fox_col = j;
            }
    if(strategy<=0) {
                // Detect first strategy case
                if ((fox_line % 2 == 0 && fox_col == 1) || fox_col == 6) {
                    bool dogs_blocking = true;
                    int k = 0;
                    if (fox_col%2==0) k = 1;
                    for (k; k < BOARD_SQUARES; k += 2) {
                        if (GameBoard[fox_line + 1][k] < DOGS) {
                            dogs_blocking = false;
                        }
                    }
                    if (dogs_blocking) {
                        strategy = 1;
                        dogIndex = 1;
                    }
                }

                // Detect second strategy case
                bool strategyTwo = false;
                if (fox_line % 2 == 0 && fox_col == 3) {
                    if(GameBoard[fox_line][fox_col-2]>=DOGS&&GameBoard[fox_line+1][fox_col-1]>=DOGS&&GameBoard[fox_line+1][fox_col+1]>=DOGS&&GameBoard[fox_line+1][fox_col+3]>=DOGS)
                        strategyTwo=true;
                }
                else if (fox_col == 4) {
                    if(GameBoard[fox_line][fox_col+2]>=DOGS&&GameBoard[fox_line+1][fox_col+1]>=DOGS&&GameBoard[fox_line+1][fox_col-1]>=DOGS&&GameBoard[fox_line+1][fox_col-3]>=DOGS)
                        strategyTwo=true;
                }

                // setting the strategy case
                if(strategyTwo) {
                    strategy = 2;
                    dogIndex = 3;
                }

                // Detect third strategy case
                bool strategyThree = false;
                if (fox_line % 2 == 0 && fox_col == 5) {
                    if(GameBoard[fox_line][fox_col-2]>=DOGS&&GameBoard[fox_line][fox_col-4]>=DOGS&&GameBoard[fox_line+1][fox_col-1]>=DOGS&&GameBoard[fox_line+1][fox_col+1]>=DOGS)
                        strategyThree=true;
                }
                else if (fox_col == 2) {
                    if(GameBoard[fox_line][fox_col+2]>=DOGS&&GameBoard[fox_line][fox_col+4]>=DOGS&&GameBoard[fox_line+1][fox_col+1]>=DOGS&&GameBoard[fox_line+1][fox_col-1]>=DOGS)
                        strategyThree=true;
                }

                // setting the strategy case
                if(strategyThree) {
                    strategy = 3;
                    dogIndex = 3;
                }
    }

    char text[100];

    switch (strategy) {
        case 1:
            sprintf(text,"Cazul 1 de strategie");
            currentMove++;
            strategy_one(currentMove,&dogIndex);

            break;
        case 2:
            sprintf(text,"Cazul 2 de strategie");
            currentMove++;
            strategy_two(currentMove,&dogIndex);
            break;
        case 3:
            sprintf(text,"Cazul 3 de strategie");
            if(currentMove==1) {
                if(GameBoard[fox_line+2][fox_col]==2) {
                    dogIndex=1;
                    currentMove+=10;
                }
                else if(GameBoard[fox_line+2][fox_col]==3)  sprintf(text,"Cazul 3 de strategie - I");
            }
            currentMove++;
            printf("Current move number: %d", currentMove);
            strategy_three(currentMove,&dogIndex);
            break;
        default:
            sprintf(text,"                                        ");
            int dog_line, dog_col;
            static int cursor_line, cursor_col;
            //static int advanceOrder,dogIndex;
            //find first dog
            if(!advance) {
                for(int i=0; i<BOARD_SQUARES; i++)
                    for(int j=0; j<BOARD_SQUARES; j++)
                        if(GameBoard[i][j]>=DOGS) {
                            dog_line = i;
                            dog_col = j;
                            break;
                        }
                
                bool dogLine = true;
                for(int index = 0; index<4; index++) {
                    if(GameBoard[dog_line][dog_col+2*index]<DOGS) {
                        dogLine = false;
                        printf("Dogs are not in line");
                        break;
                    }
                }
                if(dogLine) {
                    advanceOrder = 0;
                    cursor_line = dog_line;
                    cursor_col = 0;
                    if(dog_line % 2 == 0) {
                        advanceOrder = 1;
                        cursor_col = 7;
                    }
                }
                if(strategy==-1) {
                    for(int i = 0; i < BOARD_SQUARES; i++)
                        for(int j = 0; j < BOARD_SQUARES; j++)
                            if(GameBoard[i][j]==DOGS+dogIndex) {
                                cursor_line = i;
                                cursor_col = j;
                                break;
                            }
                    printf("Successfully changed cursor to dog index %d\n", dogIndex);
                }
                strategy = 0;
                printf("Starting the advancement of all dogs by a line\n");
                advance = true;
            }

            // Advance dogs by a line
            if (advance) {
                printf("Currently moving dog number %d.\n", dogIndex + 1);
                advanceDogs(advanceOrder, &cursor_line, &cursor_col);
            }
            break;
    }
    outtextxy((screen_width-textwidth(text))/2,20,text);
}

#pragma endregion

void checkWin() {
    // Verifică dacă vulpea a ajuns pe ultima linie
    for (int i = 0; i < BOARD_SQUARES; i++) {
        if (GameBoard[BOARD_SQUARES-1][i] == FOX) { // Linia 7
            winState = FOX;
            return;
        }
    }
    bool foxMovesLeft = false;
    // Verifică dacă vulpea mai are mutări posibile
    for (int i = 0; i < BOARD_SQUARES; i++)
        for (int j = 0; j < BOARD_SQUARES; j++)
            if (GameBoard[i][j] == FOX) // Găsește poziția vulpii
                if (movesLeft(i,j)) {
                    winState = 0; // Vulpea mai are mutări, jocul continuă
                    foxMovesLeft = true;
                }
    // Dacă nu mai are mutări, câinii au câștigat
    if(!foxMovesLeft) winState = DOGS;
    if(GameBoard[0][1]>=DOGS&&GameBoard[0][3]>=DOGS&&GameBoard[0][5]>=DOGS&&GameBoard[0][7]>=DOGS) winState = FOX;
}

int main() {
    FILE *saveFile;
    initwindow(screen_width, screen_height, "Proiect");
    char newGame;

    while (1) {
        cleardevice();
        setcolor(WHITE);

        // Display main menu
        char text[100];
        sprintf(text, "Select an option:");
        outtextxy((screen_width - textwidth(text)) / 2, 200, text);
        sprintf(text, "0 - New Game          1 - Continue Last");
        outtextxy((screen_width - textwidth(text)) / 2, screen_height/2, text);

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
                outtextxy((screen_width - textwidth(text)) / 2, 500, text);
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
                for (int i = 0; i < 8; i += 2) GameBoard[7][i] = DOGS+i/2;
                GameBoard[0][3] = FOX;
                cleardevice(); drawBoard();
                break;

            case KEY_1: // Continue Last
                char text[100];
                saveFile = fopen("save.txt", "r");
                if (saveFile == NULL) {
                    //sprintf(text,"Error: Could not open save file.");
                    //outtextxy((screen_width-textwidth(text))/2,screen_height-30,text);
                    printf("Error: Could not open save file.");
                    continue;
                }
                else {
                    fscanf(saveFile, "%d %d", &gameMode, &playerTurn);
                    for (int i = 0; i < 8; i++) {
                        for (int j = 0; j < 8; j++) {
                            fscanf(saveFile, "%d", &GameBoard[i][j]);
                        }
                    }
                    fclose(saveFile);
                    cleardevice(); drawBoard();
                    break;
                }

            default:
                printf("Error: Invalid selection.\n");
                continue;
        }

        // Game Loop
        while (1) {

            // Check for win condition
            checkWin();
            if(winState!=0) {
                cleardevice();
                char text[100];
                if(winState==FOX)  {
                    setcolor(WHITE);
                    sprintf(text,"The FOX has won!");
                    outtextxy((screen_width - textwidth(text)) / 2, screen_height / 2, text);
                }
                else if(winState==DOGS) {
                    setcolor(WHITE);
                    sprintf(text,"The DOGS have won!");
                    outtextxy((screen_width - textwidth(text)) / 2, screen_height / 2, text);
                }
            }
            else {
                switch (gameMode) {
                    case MODE_TWO_PLAYERS:
                        showTurn();
                        playerMove();
                        break;
                    case MODE_VS_CPU_RANDOM:
                        playerMove();
                        if(computerTurn) randomMove();
                        break;
                    case MODE_VS_CPU_STRATEGY:
                        playerMove();
                        if(computerTurn) mainStrategy();
                        break;
                    default:
                        break;
                }
            }
            //Test ESC key for save menu / go back to last menu / exit program
            if (kbhit()) {
                char key = getch();
                if (key == KEY_ESC) {
                    cleardevice();
                    if(winState == 0) saveGame();
                    break; // Exit game loop
                }
                //Debugging
                //sprintf(text, "You pressed: %c (%d)", key, key);
                //outtextxy((screen_width - textwidth(text)) / 2, 500, text);
            }

            delay(50);
        }
    }

    closegraph();
    return 0;
}
