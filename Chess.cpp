#include "conio.hh"
#include <stdlib.h>
#include <string.h>
#include "Chess.h"
#include "colors.h"

// #define A 97
const int A = 97;
const int D = 100;
const int W = 119;
const int S = 115;
const int quit = 81;
const int P = 112;
const int O = 111;
const int SPACE = 32;
 
const int kBoardSize = 10;
const int kFieldSize = 8;
const int kBorderDist = 2;

const char* pixelList[kBoardSize * kBoardSize];
int bgColMap[kBoardSize * kBoardSize];
int colorMap[kBoardSize * kBoardSize];


void set(int x, int y, const char* var);


#define null 0

struct Vector
{
  int x;
  int y;
};

int kWhiteTeam = 0;
int kBlackTeam = 1;

int kPawn = 1;
int kBishop = 2;
int kKnight = 3;
int kRook = 4;
int kQueen = 5;
int kKing = 6;

// set(1, 1, "♖"); 
// set(1, 2, "♘"); set(1, 3, "♗"); set(1, 4, "♕"); set(1, 5, "♔"); set(1, 6, "♗"); set(1, 7, "♘"); set(1, 8, "♖");
// set(2, 1, "♙"); set(2, 2, "♙"); set(2, 3, "♙"); set(2, 4, "♙"); set(2, 5, "♙"); set(2, 6, "♙"); set(2, 7, "♙"); set(2, 8, "♙");
    // set(8, 1, "♜"); set(8, 2, "♞"); set(8, 3, "♝"); set(8, 4, "♛"); set(8, 5, "♚"); set(8, 6, "♝"); set(8, 7, "♞"); set(8, 8, "♜");
    // set(7, 1, "♟"); set(7, 2, "♟"); set(7, 3, "♟"); set(7, 4, "♟"); set(7, 5, "♟"); set(7, 6, "♟"); set(7, 7, "♟"); set(7, 8, "♟");

typedef struct Unit{
    int side; // 0(white) or 1(black)
    int type; // Pawn, Bishop, kNight, Rook, Queen, King
    const char* emoji;

    int getTeamColor() {
        return side == kWhiteTeam ? kfCyan : kfMagenta;
    }
} Unit;

Unit whitePawn = {kWhiteTeam, kPawn, "♟"};
Unit whiteBishop = {kWhiteTeam, kBishop, "♝"};
Unit whiteKnight = {kWhiteTeam, kKnight, "♞"};
Unit whiteRook = {kWhiteTeam, kRook, "♜"};
Unit whiteQueen = {kWhiteTeam, kQueen, "♛"};
Unit whiteKing = {kWhiteTeam, kKing, "♚"};

Unit blackPawn = {kBlackTeam, kPawn, "♙"};
Unit blackBishop = {kBlackTeam, kBishop, "♗"};
Unit blackKnight = {kBlackTeam, kKnight, "♘"};
Unit blackRook = {kBlackTeam, kRook, "♖"};
Unit blackQueen = {kBlackTeam, kQueen, "♕"};
Unit blackKing = {kBlackTeam, kKing, "♔"};

Unit* map[8*8] = {
&whiteRook, &whitePawn, null, null, null, null, &blackPawn, &blackRook,
&whiteKnight, &whitePawn, null, null, null, null, &blackPawn, &blackKnight,
&whiteBishop, &whitePawn, null, null, null, null, &blackPawn, &blackBishop,
&whiteQueen, &whitePawn, null, null, null, null, &blackPawn, &blackQueen,
&whiteKing, &whitePawn, null, null, null, null, &blackPawn, &blackKing,
&whiteBishop, &whitePawn, null, null, null, null, &blackPawn, &blackBishop,
&whiteKnight, &whitePawn, null, null, null, null, &blackPawn, &blackKnight,
&whiteRook, &whitePawn, null, null, null, null, &blackPawn, &blackRook
};

bool renderBorder(int x, int y, int bgColor) {
    bool isBorder = x == 0 || y == 0 || x == kBoardSize-1 || y == kBoardSize-1;

    if (!isBorder) {
        return false;
    }
        
    printf("\x1b[%dm%s\x1b[0m", bgColor, " ");
    return true;
}

Unit* getUnitFromBoard(int x, int y) {
    return map[(y - 1) * kFieldSize + (x - 1)];
}

bool renderUnit(int x, int y, int bgColor) {
    Unit* pUnit = getUnitFromBoard(x, y);
    if (pUnit == null) return false;
    
    printf("\x1b[%d;%dm%s\x1b[0m", pUnit->getTeamColor(), bgColor, pUnit->emoji);
    return true;
}

bool isEmpty(int x, int y) {
    return getUnitFromBoard(x, y) == null;
}

void renderSpace(int x, int y, int bgColor) {
    printf("\x1b[%dm%s\x1b[0m", bgColor, " ");
}

void render() {
    for (int y=0; y<kBoardSize; y++) {
        for (int x=0; x<kBoardSize; x++) {
            int bgColor = !((x/kBorderDist+y/kBorderDist)%2) ? kbGreen : kbCyan;
            if (renderBorder(x, y, bgColor)) continue;
            bgColor = ((x+y)%2) ? kbWhite : kbBlack;
            if (renderUnit(x, y, bgColor)) continue;
            renderSpace(x, y, bgColor);
        }
        printf("\n");
    }
}

int IsGame = 1;
int delay = 16;
struct Vector pointer;
struct Vector origin;


int canMove(struct Vector v, int dx, int dy)
{
    int x = dx + v.x;
    int y = dy + v.y;

    return x != 0 && y != 0 && x != kBoardSize - 1 && y != kBoardSize - 1;
}

int getBackgroundColor(int isDark, int isBorder) 
{ 
    if (isBorder) {
        return kbRed;
    }
    return isDark ? kbBlack : kbWhite;
}

void initialize() {
    pointer.x = 1;
    pointer.y = 1;
    // memset(pixelList, " ", sizeof(pixelList) + sizeof(pixelList[0]));

    for (int i = 0; i < kBoardSize * kBoardSize; i++){
        int x = i % kBoardSize;
        int y = i / kBoardSize;
        int isDark = (i + i / kBoardSize) % 2 == 0;
        int isBorder = x == 0 || x == kBoardSize - 1 || y == 0 || y == kBoardSize - 1;

        bgColMap[i] = getBackgroundColor(isDark, isBorder);

        colorMap[i] = kfMagenta;        

        pixelList[i] = " ";
    }
}

void buildUnits() 
{
    set(1, 1, "♖"); 
    set(1, 2, "♘"); set(1, 3, "♗"); set(1, 4, "♕"); set(1, 5, "♔"); set(1, 6, "♗"); set(1, 7, "♘"); set(1, 8, "♖");
    set(2, 1, "♙"); set(2, 2, "♙"); set(2, 3, "♙"); set(2, 4, "♙"); set(2, 5, "♙"); set(2, 6, "♙"); set(2, 7, "♙"); set(2, 8, "♙");
    for (int i = 0; i < kBoardSize * kBoardSize; i++){
        if (strcmp(pixelList[i]," ") !=0 && strcmp(pixelList[i],"|")!=0 && strcmp(pixelList[i],"_")!=0){
            otherSet(i % kBoardSize, i / kBoardSize, 36, colorMap);
        }
    }
    set(8, 1, "♜"); set(8, 2, "♞"); set(8, 3, "♝"); set(8, 4, "♛"); set(8, 5, "♚"); set(8, 6, "♝"); set(8, 7, "♞"); set(8, 8, "♜");
    set(7, 1, "♟"); set(7, 2, "♟"); set(7, 3, "♟"); set(7, 4, "♟"); set(7, 5, "♟"); set(7, 6, "♟"); set(7, 7, "♟"); set(7, 8, "♟");
}

int gameLoop(){
    int underCol = kbBlack;
    const char* underUni = "♖";
    const char* catchUni = 0;
    int pointerCol = kbGreen;
    int catchCol = 0;
    int underTCol = kfCyan;
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
        

        underUni = pixelList[pointer.x + pointer.y * kBoardSize];
        if (key == SPACE){
            if (catchUni){
                int King = (!strcmp(catchUni, "♔") || !strcmp(catchUni, "♚")); // all / 2 +28
                int Queen = (!strcmp(catchUni, "♕") || !strcmp(catchUni, "♛"));
                int Bishop = (!strcmp(catchUni, "♗") || !strcmp(catchUni, "♝"));
                int Knight = (!strcmp(catchUni, "♘") || !strcmp(catchUni, "♞"));
                int Rook = (!strcmp(catchUni, "♖") || !strcmp(catchUni, "♜"));
                int Pawn = (!strcmp(catchUni, "♙") || !strcmp(catchUni, "♟"));

                int dx = origin.x - pointer.x;
                int dy = origin.y - pointer.y;
                int adxbs = abs(dx);
                int adybs = abs(dy);

                int isReticle = (origin.x == pointer.x || origin.y == pointer.y);
                int isDiagonal = (adxbs == adybs);

                int moveDistance = (adxbs > adybs) * adxbs + (adxbs < adybs) * adybs + (adxbs == adybs) * adxbs;

                float inclination_x = 0;
                float inclination_y = 0;
                
                if (adxbs < adybs){
                    if (dx != 0)
                        inclination_x = (float)dx / (float)adybs;
                    if (dy != 0)
                        inclination_y = dy / adybs;
                } else{
                    if (dx != 0)
                        inclination_x = dx / adxbs;
                    if (dy != 0)
                        inclination_y = (float)dy / (float)adxbs;
                }

                int i = 1;
                for (;i<moveDistance; i++){
                    int index = indexOf(pointer.x + (int)((float)i * inclination_x), pointer.y + (int)((float)i * inclination_y));
                    // bgColMap[index] = kbGreen;
                    if (strcmp(pixelList[index], " ")){
                        break;
                    }
                }
                
                int cancollide = (i != moveDistance); //Knight와 같이 기울기가 1이하가 돼는 움직임은 충돌 검사를 두번.
                
                // if (!cancollide) {
                //     for (;i<moveDistance; i++){
                //         if (strcmp(pixelList[indexOf(pointer.x + (int)((float)i * inclination_x), pointer.y + (int)((float)i * inclination_y))], " ")){
                            
                //             break;
                //         }
                //     }               
                // }

                int pass = 0;
                // printf("x, y : %f, %f, %d, %d, cancollide : %d, %d\n", inclination_x, inclination_y, dx, dy, cancollide, moveDistance);
                // return 0;

                if (King) {
                    if ((isReticle || isDiagonal) && (adxbs == 1 || adybs == 1)) // 터치무브 규칙 쓰려면 (isReticle 과 isDiagonal 사이에) ^ 추가. 없에려면 || 추가
                        pass = 1;
                } else if (Queen) {
                    if ((isReticle || isDiagonal) && !cancollide) // 터치무브 규칙 쓰려면 (isReticle 과 isDiagonal 사이에) ^ 추가. 없에려면 || 추가
                        pass = 1;
                } else if (Bishop) {
                    if (isDiagonal && !cancollide) // 터치무브 규칙 쓰려면 && dx != 0 추가 또는 !isReticle. 규칙을 없에려면 isDiagonal만 
                        pass = 1;
                } else if (Knight) {
                    if (!cancollide && ((adxbs == 2 && adybs == 1) || (adxbs == 1 && adybs == 2)))
                        pass = 1;
                } else if (Rook) {
                    if (isReticle && !cancollide) // 터치무브 규칙 쓰려면 && dx != 0 추가 또는 && !isDiagonal. 규칙을 없에려면 isDiagonal만 
                        pass = 1;
                } else if (Pawn) {
                    if (!cancollide && ((((CatchUniTeam * 2 -1) * ((origin.x == 2 || origin.x == 7) +1) == dx || CatchUniTeam * 2 -1 == dx) && adybs == (strcmp(underUni, " ")!=0)) || dx == 0 && dy == 0))
                        pass = 1;
                }

                if (moveDistance == 0) // 전체 터치무브 규칙 적용 개별적용(X)
                    pass = 1;

                if (pass) {
                    set(pointer.x, pointer.y, catchUni);
                    otherSet(pointer.x, pointer.y, catchCol, colorMap);
                    underUni = catchUni;
                    catchUni = 0;
                    pointerCol = kbGreen;
                    catchCol = 0;
                }
            } else {
                if (strcmp(pixelList[pointer.x + pointer.y * kBoardSize], " ")){
                    catchUni = pixelList[pointer.x + pointer.y * kBoardSize];
                    pointerCol = kbYellow;
                    underUni = " ";
                    catchCol = colorMap[pointer.x + pointer.y * kBoardSize];
                    origin.x = pointer.x;
                    origin.y = pointer.y;
                    CatchUniTeam = 1;
                    if (colorMap[pointer.x + pointer.y * kBoardSize] == kfCyan)
                        CatchUniTeam = 0;
                }
            }
        }
        

        underCol = bgColMap[pointer.x + pointer.y * kBoardSize];
        underTCol = colorMap[pointer.x + pointer.y * kBoardSize];
        otherSet(pointer.x, pointer.y, catchCol, colorMap);
        otherSet(pointer.x, pointer.y, pointerCol, bgColMap);
        if (catchUni){
            set(pointer.x, pointer.y, catchUni);
        }

        // display
        printf("%s\n", (pixelList[pointer.x + pointer.y * kBoardSize]));
        printf("%d, %d\n" ,pointer.x, pointer.y);
        printf(" \n");

        draw();
        printf("\n\n");
        waitMs(delay);
        system("clear");
    } while (IsGame);
    
    return 0;
}

void draw(){
    for (int i = 0; i < kBoardSize * kBoardSize; i++){
        printf("\x1b[%d;%dm%s\x1b[0m", colorMap[i], bgColMap[i], pixelList[i]);
        if ((i + 1) % kBoardSize == 0)
            printf("\n");
    }
}

void set(int x, int y, const char* var)
{
    pixelList[x + kBoardSize * y] = var;
}

void otherSet(int x, int y, int var, int *list)
{
    list[x + kBoardSize * y] = var;
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
    return y * kBoardSize + x;
}

int main()
{
    render();

    return 0;

    initialize();



    buildUnits();



    return gameLoop();
}
