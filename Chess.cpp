#include "conio.hh"
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "Chess.h"
#include "colors.h"

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

const int delay = 1000 / 25; // 25 fps


#define null 0

struct Vector
{
  int x;
  int y;

    bool operator==(const struct Vector &v) const {
		return v.x == this->x && v.y == this->y;
	}
};

bool isQuit = false;

int kWhiteTeam = 0;
int kBlackTeam = 1;

int kPawn = 1;
int kBishop = 2;
int kKnight = 3;
int kRook = 4;
int kQueen = 5;
int kKing = 6;


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

Vector cursor = {0, 0};
int kNormalCursorColor = kbGreen;
int kGrapCusrsorColor = kbYellow;
int isCursorGraping = 0;

bool renderBorder(int x, int y) {
    bool isBorder = x == 0 || y == 0 || x == kBoardSize-1 || y == kBoardSize-1;

    if (!isBorder) {
        return false;
    }

    int bgColor = !((x/kBorderDist+y/kBorderDist)%2) ? kbBlue : kbCyan;
        
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

long getTimeMs() {
    struct timeval t;
    gettimeofday(&t, NULL);

    return t.tv_sec * 1000 + t.tv_usec/1000;
}

int cursorTime = 10;
int stopTime = getTimeMs();

int getBgColor(int x, int y) {
    Vector position = {x, y};
    if (position == cursor) {

        if (cursorTime > 0){
            cursorTime--;
            if (cursorTime == 0){
                stopTime = getTimeMs();
            }
            return isCursorGraping ? kGrapCusrsorColor : kNormalCursorColor;
        } else if ((getTimeMs() - stopTime) % 1000 < 500){
            return isCursorGraping ? kGrapCusrsorColor : kNormalCursorColor;
        }
    }

    return ((x+y)%2) ? kbWhite : kbBlack;
}

void render() {
    for (int y=0; y<kBoardSize; y++) {
        for (int x=0; x<kBoardSize; x++) {
            if (renderBorder(x, y)) continue;

            int bgColor = getBgColor(x - 1, y - 1);
            if (renderUnit(x, y, bgColor)) continue;
            renderSpace(x, y, bgColor);
        }
        printf("\n");
    }
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

void updateCursor(int dx, int dy){
    struct Vector c = {cursor.x + dx, cursor.y + dy};
    // cursor.x -= cursor.x + dx != -1 && cursor.x + dx != kFieldSize && cursor.y + dy != -1 && cursor.y + dy != kFieldSize;
    if (0<=c.x && c.x<kFieldSize && 0<= c.y && c.y<kFieldSize) {
        cursor.x = c.x;
        cursor.y = c.y;
    }
    cursorTime = 20;
}

int updateKey() {
    int key = getKeyInput();

    switch (key) {
        case A:
            updateCursor(-1, 0);
            break;
        case D:
            updateCursor(+1, 0);
            break;
        case W:
            updateCursor(0, -1);
            break;
        case S:
            updateCursor(0, 1);
            break;
    }

    return key;
}

int game(){
    do{
        if (updateKey() == quit){
            isQuit = true;
            continue;
        }   
        
        system("clear");
        
        render();

        waitMs(delay);
        
    } while (!isQuit);


    return 0;
}


int main()
{
    return game();
}
