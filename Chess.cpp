#include "conio.hh"
#include <stdlib.h>
#include <cstring>
#include "Chess.h"

// #define A 97
const int A = 97;
const int D = 100;
const int W = 119;
const int S = 115;
const int quit = 81;
const int P = 112;
const int O = 111;
const int SPACE = 32;

const int width = 10;
const int height = 10;
char* pixelList[width * height];
int bgColMap[width * height];
int colorMap[width * height];

struct Vector
{
  int x = 0;
  int y = 0;
};
int IsGame = 1;
int delay = 70;
struct Vector pointer;
struct Vector origin;


int canMove(struct Vector v, int dx, int dy)
{
    int x = dx + v.x;
    int y = dy + v.y;

    return x != 0 && y != 0 && x != width - 1 && y != height - 1;
}

int getBackgroundColor(int isDark, int isBorder) 
{   
    if (isBorder) {
        return 41;
    }
    return isDark ? 40 : 47;
}

void initialize() {
    pointer.x = 1;
    pointer.y = 1;
    // memset(pixelList, " ", sizeof(pixelList) + sizeof(pixelList[0]));

    for (int i = 0; i < width * height; i++){
        int x = i % width;
        int y = i / width;
        int isDark = (i + i / width) % 2 == 0;
        int isBorder = x == 0 || x == width - 1 || y == 0 || y == height - 1;

        bgColMap[i] = getBackgroundColor(isDark, isBorder);

        colorMap[i] = 35;        

        pixelList[i] = " ";
    }
}

void buildUnits() 
{
    set(1, 1, "♖"); 
    set(1, 2, "♘"); set(1, 3, "♗"); set(1, 4, "♕"); set(1, 5, "♔"); set(1, 6, "♗"); set(1, 7, "♘"); set(1, 8, "♖");
    set(2, 1, "♙"); set(2, 2, "♙"); set(2, 3, "♙"); set(2, 4, "♙"); set(2, 5, "♙"); set(2, 6, "♙"); set(2, 7, "♙"); set(2, 8, "♙");
    for (int i = 0; i < width * height; i++){
        if (pixelList[i] != " " && pixelList[i] != "|" && pixelList[i] != "_"){
            otherSet(i % width, i / width, 36, colorMap);
        }
    }
    set(8, 1, "♜"); set(8, 2, "♞"); set(8, 3, "♝"); set(8, 4, "♛"); set(8, 5, "♚"); set(8, 6, "♝"); set(8, 7, "♞"); set(8, 8, "♜");
    set(7, 1, "♟"); set(7, 2, "♟"); set(7, 3, "♟"); set(7, 4, "♟"); set(7, 5, "♟"); set(7, 6, "♟"); set(7, 7, "♟"); set(7, 8, "♟");
}

int gameLoop(){
    int underCol = 40;
    char* underUni = "♖";
    char* catchUni = 0;
    int pointerCol = 42;
    int catchCol = 0;
    int underTCol = 36;
    int CatchUniTeam = 0;

    do
    {

        int key = getKeyInput();
        if (key == quit)
            return 0;

        otherSet(pointer.x, pointer.y, underTCol, colorMap);
        otherSet(pointer.x, pointer.y, underCol, bgColMap);
        set(pointer.x, pointer.y, underUni);

        // key
        if (key == A && canMove(pointer, -1, 0)){
            pointer.x--;
        } else if (key == D && canMove(pointer, 1, 0)){
            pointer.x++;
        } else if (key == S && canMove(pointer, 0, 1)){
            pointer.y++;
        } else if (key == W && canMove(pointer, 0, -1))
            pointer.y--;
        

        underUni = pixelList[pointer.x + pointer.y * width];
        if (key == SPACE){
            if (catchUni){
                int King = (catchUni == "♔" || catchUni == "♚"); // all / 2 +28
                int Queen = (catchUni == "♕" || catchUni == "♛");
                int Bishop = (catchUni == "♗" || catchUni == "♝");
                int Knight = (catchUni == "♘" || catchUni == "♞");
                int Rook = (catchUni == "♖" || catchUni == "♜");
                int Pawn = (catchUni == "♙" || catchUni == "♟");

                int dx = origin.x - pointer.x;
                int dy = origin.y - pointer.y;

                int isReticle = (origin.x == pointer.x || origin.y == pointer.y);
                int isDiagonal = (abs(dx) == abs(dy));

                int pass = 0;
                if (King) {
                    if ((isReticle ^ isDiagonal) && (abs(dx) == 1 || abs(dy) == 1))
                        pass = 1;
                } else if (Queen) {
                    if (isReticle ^ isDiagonal)
                        pass = 1;
                } else if (Bishop) {
                    if (isDiagonal && dx != 0)
                        pass = 1;
                } else if (Knight) {
                    if ((abs(dx) == 2 && abs(dy) == 1) || (abs(dx) == 1 && abs(dy) == 2))
                        pass = 1;
                } else if (Rook) {
                    if (isReticle && !isDiagonal)
                        pass = 1;
                } else if (Pawn) {
                    if ((((CatchUniTeam * 2 -1) * ((origin.x == 2 || origin.x == 7) +1) == dx || CatchUniTeam * 2 -1 == dx) && abs(dy) == (underUni != " ")) || dx == 0 && dy == 0)
                        pass = 1;
                }

                if (pass) {
                    set(pointer.x, pointer.y, catchUni);
                    otherSet(pointer.x, pointer.y, catchCol, colorMap);
                    underUni = catchUni;
                    catchUni = 0;
                    pointerCol = 42;
                    catchCol = 0;
                }
            } else {
                if (pixelList[pointer.x + pointer.y * width] != " "){
                    catchUni = pixelList[pointer.x + pointer.y * width];
                    pointerCol = 43;
                    underUni = " ";
                    catchCol = colorMap[pointer.x + pointer.y * width];
                    origin.x = pointer.x;
                    origin.y = pointer.y;
                    CatchUniTeam = 1;
                    if (colorMap[pointer.x + pointer.y * width] == 36)
                        CatchUniTeam = 0;
                }
            }
        }
        

        underCol = bgColMap[pointer.x + pointer.y * width];
        underTCol = colorMap[pointer.x + pointer.y * width];
        otherSet(pointer.x, pointer.y, catchCol, colorMap);
        otherSet(pointer.x, pointer.y, pointerCol, bgColMap);
        if (catchUni){
            set(pointer.x, pointer.y, catchUni);
        }

        // display
        printf("%d\n", pixelList[pointer.x + pointer.y * width]);
        printf("%d, %d\n" ,pointer.x, pointer.y);

        draw();
        printf("\n\n");
        waitMs(delay);
        system("clear");
    } while (IsGame);
    
    return 0;
}

void draw(){
    for (int i = 0; i < width * height; i++){
        printf("\x1b[%d;%dm%s\x1b[0m", colorMap[i], bgColMap[i], pixelList[i]);
        if ((i + 1) % width == 0)
            printf("\n");
    }
}

void set(int x, int y, char* var)
{
    pixelList[x + width * y] = var;
}

void otherSet(int x, int y, int var, int *list)
{
    list[x + width * y] = var;
}

int getKeyInput()
{
    if (kbhit() == 1)
        return getchar();
    else
        return 0;
}

void waitMs(int milliseconds)
{
    usleep(milliseconds * 1000);
}

int indexOf(int x, int y)
{
    return y * width + x;
}

int main()
{
    initialize();



    buildUnits();



    return gameLoop();
}
