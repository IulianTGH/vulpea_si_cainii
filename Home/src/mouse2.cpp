#include <winbgim.h>
#include <graphics.h>
#include <stdio.h>
#include <math.h>

int left, top, size, x, y, latura;

void punePiesa() {
    if(ismouseclick(WM_LBUTTONDOWN)) {
        clearmouseclick(WM_LBUTTONDOWN);
        int linie = (y-top)/latura;
        int coloana = (x-left)/latura;
        if((left<x&&x<left+size)&&(top<y&&y<top+size))  circle(left+coloana*latura+latura/2,top+linie*latura+latura/2,latura/4);
    }
}

void desen() {
    size=400; //dimensiunea tablei
    int numar=8; //numarul de patrate pentru tabla
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

int main() {
    initwindow(800,600,"Program mouse rudimentar");
    char text[100],toptxt[50],lefttxt[50];
    desen();
    sprintf(toptxt,"Top: %d",top);
    sprintf(lefttxt,"Left: %d",left);
    while (1)
    {
        x=mousex(); y=mousey();
        punePiesa();
        sprintf(text,"X:%d,Y:%d",x,y);
        outtextxy(380,580,text);
        outtextxy(10,260,toptxt);
        outtextxy(10,280,lefttxt);
        int linie = (y-top)/latura;
        int coloana = (x-left)/latura;
        char l[50],c[50];
        if((left<x&&x<left+size)&&(top<y&&y<top+size))  {
            sprintf(l,"Linia: %d", linie);  sprintf(c,"Coloana: %d", coloana);
            outtextxy(720,260,l);   outtextxy(720,280,c);
        }
        if(kbhit()) {
            char key = getch();
            if(key==27) break;
        }
        delay(50);
    }
    closegraph();   return 0;
}