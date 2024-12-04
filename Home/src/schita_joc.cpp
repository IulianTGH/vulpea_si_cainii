#include <winbgim.h>
#include <graphics.h>
#include <stdio.h>
#include <math.h>

int left, top, size, x, y, xv, yv, latura, numar, linie, coloana, old_line, old_column, win=0;

int TablaDeJoc[8][8] = {0};

int mutariPosibile(int line, int column) {
    int mutari = 0;
    // Verifică diagonală sus-stânga
    if (line > 1 && column > 1 && TablaDeJoc[line - 1][column - 1] == 0)    mutari++;
    // Verifică diagonală sus-dreapta
    if (line > 1 && column < 8 && TablaDeJoc[line - 1][column + 1] == 0)    mutari++;
    // Verifică diagonală jos-stânga
    if (line < 8 && column > 1 && TablaDeJoc[line + 1][column - 1] == 0)    mutari++;
    // Verifică diagonală jos-dreapta
    if (line < 8 && column < 8 && TablaDeJoc[line + 1][column + 1] == 0)    mutari++;
    return mutari;
}


void gameFinish() {
    // Verifică dacă vulpea a ajuns pe ultima linie
    for (int i = 1; i <= numar; i++) {
        if (TablaDeJoc[numar][i] == 1) { // Linia 8
            win = 1; // Vulpea a câștigat
            return;
        }
    }
    // Verifică dacă vulpea mai are mutări posibile
    for (int i = 1; i <= numar; i++)
        for (int j = 1; j <= numar; j++)
            if (TablaDeJoc[i][j] == 1) { // Găsește poziția vulpii
                int line = i;
                int column = j;
                if (mutariPosibile(line,column) > 0)
                    return; // Vulpea mai are mutări, jocul continuă
            }
    // Dacă nu mai are mutări, câinii au câștigat
    win = 2;
}


void drawPiece(int center_x, int center_y, int culoare) {
    setfillstyle(SOLID_FILL, culoare);
    fillellipse(center_x+latura/2,center_y+latura/2,latura/4,latura/4);
}

void seteazaTabla() {
    int coloana_vulpe = 4; int linie_vulpe = 1;
    int coloana_caini, linie_caini = 8;
    for(coloana_caini=1; coloana_caini<=8; coloana_caini++) {
        if(coloana_caini%2==1) {
            int xcc = left+(coloana_caini-1)*latura;   int ycc = top+(linie_caini-1)*latura;
            drawPiece(xcc,ycc,2);
            TablaDeJoc[linie_caini][coloana_caini]=2;
        }
    }
    int xcc = left+(coloana_vulpe-1)*latura;   int ycc = top+(linie_vulpe-1)*latura;
    drawPiece(xcc,ycc,1);
    TablaDeJoc[linie_vulpe][coloana_vulpe]=1;
}

bool checkMove(int *player) {
    if(TablaDeJoc[linie][coloana]!=0) return false;
    if((left>x||x>left+8*latura)||(top>y||y>top+8*latura)) return false;
    if(old_line-linie==1&&abs(coloana-old_column)==1) return true;
    if(*player==1)
    if(abs(old_line-linie)==1&&abs(coloana-old_column)==1) return true;
    return false;
}

void schimbarePozitie(int *player, int old_line, int old_column) {
    linie = (y-top)/latura+1;   coloana = (x-left)/latura+1;
    if(checkMove(player)) {
    int xc = left+(coloana-1)*latura;   int yc = top+(linie-1)*latura;
    drawPiece(xc,yc,*player);
    TablaDeJoc[linie][coloana]=*player;
    TablaDeJoc[old_line][old_column]=0;
    setfillstyle(SOLID_FILL, BLACK);
    bar(left+(old_column-1)*latura+5,top+(old_line-1)*latura+5,left+(old_column)*latura-5,top+old_line*latura-5);
    if(*player==1) *player=2;
    else *player=1;
}
}

void mutaPiesa(int* player) {
    char p[20]; sprintf(p,"Jucator %d",*player);
    outtextxy(380,20,p);
    if(ismouseclick(WM_LBUTTONDOWN)) {
        clearmouseclick(WM_LBUTTONDOWN);
        xv = mousex(); yv = mousey();
        linie = (yv-top)/latura+1;   coloana = (xv-left)/latura+1;
        old_line=linie; old_column=coloana;
        if(TablaDeJoc[linie][coloana]==*player) {
        while(1) {
            if(ismouseclick(WM_LBUTTONDOWN)) {
                clearmouseclick(WM_LBUTTONDOWN);
                x = mousex(); y = mousey();
                schimbarePozitie(player,old_line,old_column);
                gameFinish();
                break;
                }
            }
        }
    }
}

void desenTabla() {
    size=400; //dimensiunea tablei
    numar=8; //numarul de patrate pentru tabla
    latura=size/numar; //lungimea laturii unui patrat
    //stabilirea pozitiei tablei
    top=(getmaxy()-size+1)/2;
    left=(getmaxx()-size+1)/2;
    //desenarea tablei
    for(int i=1; i<=numar; i++)
    for (int j=1; j<=numar; j++)
    {
        rectangle(left+latura*(j-1),top+latura*(i-1),left+latura*j,top+latura*i);
    }
    
}

void afisareMatrice() {
    for (int i=1; i<=8; i++) {
    for (int j=1; j<=8; j++) {
        printf("%d ",TablaDeJoc[i][j]);
    }
    printf("\n");
    }
}

void debugInfo() {
    int dx=mousex(); int dy=mousey();
    char text[100],toptxt[50],lefttxt[50];
    sprintf(toptxt,"Top: %d",top);
    sprintf(lefttxt,"Left: %d",left);
    sprintf(text,"X:%d,Y:%d",dx,dy);
    outtextxy(380,580,text);
    outtextxy(10,260,toptxt);
    outtextxy(10,280,lefttxt);
    char l[50],c[50];
    if((left<dx&&dx<left+size)&&(top<dy&&dy<top+size))  {
        sprintf(l,"Linia: %d", linie+1);  sprintf(c,"Coloana: %d", coloana+1);
        outtextxy(720,260,l);   outtextxy(720,280,c);
    }
    if(kbhit()){
        char key = getch();
        if(key==32) afisareMatrice();
        else{
        outtextxy(380,560,"                                                ");
        char keyText[100];
        sprintf(keyText,"Ai apasat tasta: %c(%d)",key,key);
        outtextxy(380,560,keyText);
        }
    }
}

int main() {
    initwindow(800, 600, "Program mouse rudimentar");
    desenTabla();
    seteazaTabla();
    int jucator = 1;
    while (1) {
        int ok=0;
        // Verifică dacă jocul s-a terminat
        gameFinish();
        // Dacă cineva a câștigat, afișează mesajul și ieși din buclă
        if (win==1&&ok==0) {
            outtextxy(380, 20, "Vulpea a castigat!");
            ok=1;
        }
        if (win==2&&ok==0) {
            outtextxy(380, 20, "Cainii au castigat!");
            ok=1;
        }
        if(!win)    mutaPiesa(&jucator);
        // Verifică input de la tastatură pentru a ieși
        if (kbhit()) {
            char key = getch();
            if (key == 27) break; // Apasă ESC pentru a ieși
        }
        delay(50);
    }
    closegraph();   return 0;
}
