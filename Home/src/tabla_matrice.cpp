#include <winbgim.h>
#include <graphics.h>
#include <stdio.h>
#include <math.h>

#define MAX 8

int left, top, size, x, y, latura, numar;

int TablaDeJoc[MAX][MAX] = {0};

bool mutareValida() {
    if((left<x&&x<left+size)&&(top<y&&y<top+size)) return 1;
    return 0;
}

void punePiesa(int* player) {
    if(ismouseclick(WM_LBUTTONDOWN)) {
        clearmouseclick(WM_LBUTTONDOWN);
        x = mousex(); y = mousey();
        int linie = (y-top)/latura+1;   int coloana = (x-left)/latura+1;
        int xc = left+(coloana-1)*latura;   int yc = top+(linie-1)*latura;
        if(mutareValida()&&TablaDeJoc[linie][coloana]==0) {
        TablaDeJoc[linie][coloana]=*player;
        circle(xc+latura/2,yc+latura/2,latura/4);
        if(*player==1) *player=2;
        else *player=1;
        }
    }
}

void desen() {
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

void debugInfo() {
    int dx=mousex(); int dy=mousey();
    char text[100],toptxt[50],lefttxt[50];
    sprintf(toptxt,"Top: %d",top);
    sprintf(lefttxt,"Left: %d",left);
    sprintf(text,"X:%d,Y:%d",dx,dy);
    outtextxy(380,580,text);
    outtextxy(10,260,toptxt);
    outtextxy(10,280,lefttxt);
    int linie = (dy-top)/latura+1;
    int coloana = (dx-left)/latura+1;
    char l[50],c[50];
    if((left<dx&&dx<left+size)&&(top<dy&&dy<top+size))  {
        sprintf(l,"Linia: %d", linie);  sprintf(c,"Coloana: %d", coloana);
        outtextxy(720,260,l);   outtextxy(720,280,c);
    }
}

void afisareMatrice() {
    for (int i=1; i<=numar; i++) {
    for (int j=1; j<=numar; j++) {
        printf("%d\t",TablaDeJoc[i][j]);
    }
    printf("\n");
    }
}

int main() {
    initwindow(800,600,"Program mouse rudimentar");
    desen();
    int jucator = 1;
    while (1)
    {
        debugInfo(); // modul pentru depanare
        punePiesa(&jucator);
        if(kbhit()) {
            outtextxy(380,560,"                                                ");
            char keyText[100];
            char key = getch();
            sprintf(keyText,"Ai apasat tasta: %c(%d)",key,key);
            outtextxy(380,560,keyText);
            if(key==27) break;
            if(key==32) afisareMatrice();

        }
        delay(50);
    }
    closegraph();   return 0;
}