// hua-rong-dao.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cassert>
#include <string>
#include <vector>
#include <deque>
#include <set>
#include <iostream>
#include <iterator>
#include <conio.h>
#include <ctime>

#define NO_LR_MIRROR_ALLOW            1

const int MAX_HERO_COUNT = 10;
const int MAX_MOVE_DIRECTION = 4;
const char MAX_WARRIOR_TYPE = 5;
const int HRD_GAME_ROW = 5;
const int HRD_GAME_COL = 4;
const int HRD_BOARD_WIDTH = 6;
const int HRD_BOARD_HEIGHT = 7;
const int CAO_ESCAPE_LEFT = 1;
const int CAO_ESCAPE_TOP = 3;

const char BOARD_CELL_EMPTY = 0;
const char BOARD_CELL_BORDER = 0x0F;

typedef enum tagWARRIOR_TYPE
{
    HT_BLOCK = 1,  //1x1
    HT_VBAR = 2,   //1x2
    HT_HBAR = 3,   //2x1
    HT_BOX = 4     //2x2
}WARRIOR_TYPE;

typedef struct tagWARRIOR
{
    WARRIOR_TYPE type;
    int left;
    int top;
}WARRIOR;

typedef struct tagDIRECTION
{
    int hd;
    int vd;
}DIRECTION;

typedef struct tagMOVE_ACTION
{
    int heroIdx;
    int dirIdx;
}MOVE_ACTION;

typedef struct tagSTART_POSITION
{
    std::string startName;
    int heroCount;
    int caoIdx;
    char *heroName[MAX_HERO_COUNT];
    int heroInfo[MAX_HERO_COUNT * 3];
}START_POSITION;

struct HRD_GAME_STATE
{
    char board[HRD_BOARD_HEIGHT][HRD_BOARD_WIDTH];
    std::vector<WARRIOR> heroes;
    MOVE_ACTION move;
    int step;
    unsigned int hash;
    HRD_GAME_STATE *parent;
};

typedef struct tagHRD_GAME
{
    std::string gameName;
    std::vector<std::string> heroNames;
    char caoIdx;
    std::deque<HRD_GAME_STATE *> states;
    std::set<unsigned int> zhash;
    int result;
}HRD_GAME;

typedef struct tagCellState
{
    int value[MAX_WARRIOR_TYPE];
}CELL_STATE;

typedef struct tagZobristHash
{
    CELL_STATE key[HRD_GAME_ROW][HRD_GAME_COL];
}ZOBRIST_HASH;

DIRECTION directions[MAX_MOVE_DIRECTION] = { {0, -1}, {1, 0}, {0, 1}, {-1, 0} };
const char *dir_string[MAX_MOVE_DIRECTION] = {"上", "右", "下", "左"};
ZOBRIST_HASH zob_hash;

/*4种解，最快81步*/
START_POSITION HengDaoLiMa = 
{
    "横刀立马", 10, 2,
    { "赵云", "曹操", "黄忠", "马超", "关羽", "张飞", "勇", "兵", "卒", "士" },
    { 2,0,0, 4,1,0, 2,3,0, 2,0,2, 3,1,2, 2,3,2, 1,0,4, 1,1,3, 1,2,3, 1,3,4 }
};

/*4种解，最快73步*/
START_POSITION ZhihuiRuoDing = 
{
    "指挥若定", 10, 2,
    { "赵云", "曹操", "黄忠", "兵", "关羽", "卒", "马超", "勇", "士", "张飞" },
    { 2,0,0,  4,1,0,   2,3,0,  1,0,2, 3,1,2,   1,3,2, 2,0,3, 1,1,3, 1,2,3, 2,3,3 }
};

/*21种解，最快28步*/
START_POSITION BiYiHengKong = 
{
    "比翼横空", 10, 3,
    { "关羽", "张飞", "曹操", "马超", "黄忠", "兵", "勇",   "卒",  "士", "赵云" },
    { 3,0,0,  3,0,1,   4,2,0,  3,0,2,   3,2,2,   1,0,3, 1,0,4, 1,2,3, 1,2,4, 2,3,3 }
};

/*4种解，最快74步*/
START_POSITION BingFenSanLu = 
{
    "兵分三路", 10, 2,
    { "兵", "曹操", "卒",  "赵云", "关羽", "黄忠", "马超",  "勇",  "士", "张飞" },
    { 1,0,0,  4,1,0, 1,3,0, 2,0,1,   3,1,2,   2,3,1,  2,0,3,   1,1,3, 1,2,3, 2,3,3 }
};

/*21种解，最快88步*/
START_POSITION SiHengYiShuSiBing = 
{
    "四横一竖四兵", 10, 2,
    { "赵云", "曹操", "兵",  "卒",   "勇",  "关羽", "士",   "张飞",  "马超", "黄忠" },
    { 2,0,0,  4,1,0,   1,3,0,  1,3,1,  1,0,2, 3,1,2,   1,3,2,  3,0,3,   3,2,3,   3,1,4 }
};

bool IsReverseDirection(int dirIdx1, int dirIdx2)
{
    return ( ((dirIdx1 + 2) % MAX_MOVE_DIRECTION) == dirIdx2);
}

void CopyGameState(HRD_GAME_STATE* from, HRD_GAME_STATE* to)
{
    memcpy(to->board, from->board, HRD_BOARD_HEIGHT * HRD_BOARD_WIDTH);
    copy(from->heroes.begin(), from->heroes.end(), back_inserter(to->heroes));
}

std::string DirectionStringFromIndex(int dirIdx)
{
    assert( (dirIdx >= 0) && (dirIdx < MAX_MOVE_DIRECTION) );

    return std::string(dir_string[dirIdx]);
}

unsigned int GetZobristHash(ZOBRIST_HASH *zob_hash, HRD_GAME_STATE *state)
{
    unsigned int hash = 0;
    const std::vector<WARRIOR>& heroes = state->heroes;
    for(int i = 1; i <= HRD_GAME_ROW; i++) 
    {
        for(int j = 1; j <= HRD_GAME_COL; j++) 
        {
            int index = state->board[i][j] - 1;
            int type = (index >= 0 && index < heroes.size()) ? heroes[index].type : 0;
            hash ^= zob_hash->key[i - 1][j - 1].value[type];
        }
    }
    return hash;
}

unsigned int GetMirrorZobristHash(ZOBRIST_HASH *zob_hash, HRD_GAME_STATE *state)
{
    unsigned int hash = 0;
    const std::vector<WARRIOR>& heroes = state->heroes;
    for(int i = 1; i <= HRD_GAME_ROW; i++) 
    {
        for(int j = 1; j <= HRD_GAME_COL; j++) 
        {
            int index = state->board[i][j] - 1;
            int type = (index >= 0 && index < heroes.size()) ? heroes[index].type : 0;

            //(HRD_GAME_COL - 1) - (j - 1))
            hash ^= zob_hash->key[i - 1][HRD_GAME_COL - j].value[type];
        }
    }
    return hash;
}
/*
unsigned int GetZobristHashUpdate(ZOBRIST_HASH *zob_hash, HRD_GAME_STATE *state, int heroIdx, int dirIdx)
{
    unsigned int hash = state->hash;
    const WARRIOR& hero = gameState->heroes[heroIdx];
    const DIRECTION& dir = directions[dirIdx];

    switch(hero.type)
    {
        ...
    case HT_VBAR:
        //原始位置的处理：
        hash ^= zob_hash->key[hero.left][hero.top].value[hero.type];
        hash ^= zob_hash->key[hero.left][hero.top + 1].value[hero.type];
        hash ^= zob_hash->key[hero.left][hero.top].value[0]; //0是空状态
        hash ^= zob_hash->key[hero.left][hero.top + 1].value[0];
        //新位置的处理：
        hash ^= zob_hash->key[hero.left + dir.hd][hero.top + dir.vd].value[0];//0是空状态
        hash ^= zob_hash->key[hero.left + dir.hd][hero.top + dir.vd + 1].value[0];
        hash ^= zob_hash->key[hero.left + dir.hd][hero.top + dir.vd].value[hero.type]; 
        hash ^= zob_hash->key[hero.left + dir.hd][hero.top + dir.vd + 1].value[hero.type];
        break;
        ...
    }

    return hash;
}
*/
bool IsPositionAvailable(HRD_GAME_STATE* gameState, WARRIOR_TYPE type, int left, int top)
{
    bool isOK = false;

    switch(type)
    {
    case HT_BLOCK:
        isOK = (gameState->board[top + 1][left + 1] == BOARD_CELL_EMPTY);
        break;
    case HT_VBAR:
        isOK = (gameState->board[top + 1][left + 1] == BOARD_CELL_EMPTY)
               && (gameState->board[top + 2][left + 1] == BOARD_CELL_EMPTY);
        break;
    case HT_HBAR:
        isOK = (gameState->board[top + 1][left + 1] == BOARD_CELL_EMPTY)
               && (gameState->board[top + 1][left + 2] == BOARD_CELL_EMPTY);
        break;
    case HT_BOX:
        isOK = (gameState->board[top + 1][left + 1] == BOARD_CELL_EMPTY)
               && (gameState->board[top + 1][left + 2] == BOARD_CELL_EMPTY)
               && (gameState->board[top + 2][left + 1] == BOARD_CELL_EMPTY)
               && (gameState->board[top + 2][left + 2] == BOARD_CELL_EMPTY);
        break;
    default:
        isOK = false;
        break;
    }

    return isOK;
}

bool CanHeroMove(HRD_GAME_STATE* gameState, int heroIdx, int dirIdx)
{
    int cv1,cv2,cv3,cv4;
    bool canMove = false;
    const WARRIOR& hero = gameState->heroes[heroIdx];
    const DIRECTION& dir = directions[dirIdx];

    switch(hero.type)
    {
    case HT_BLOCK:
        canMove = (gameState->board[hero.top + dir.vd + 1][hero.left + dir.hd + 1] == BOARD_CELL_EMPTY);
        break;
    case HT_VBAR:
        cv1 = gameState->board[hero.top + dir.vd + 1][hero.left + dir.hd + 1];
        cv2 = gameState->board[hero.top + dir.vd + 2][hero.left + dir.hd + 1];
        canMove = (cv1 == BOARD_CELL_EMPTY || cv1 == heroIdx + 1) && (cv2 == BOARD_CELL_EMPTY || cv2 == heroIdx + 1);
        break;
    case HT_HBAR:
        cv1 = gameState->board[hero.top + dir.vd + 1][hero.left + dir.hd + 1];
        cv2 = gameState->board[hero.top + dir.vd + 1][hero.left + dir.hd + 2];
        canMove = (cv1 == BOARD_CELL_EMPTY || cv1 == heroIdx + 1) && (cv2 == BOARD_CELL_EMPTY || cv2 == heroIdx + 1);
        break;
    case HT_BOX:
        cv1 = gameState->board[hero.top + dir.vd + 1][hero.left + dir.hd + 1]; 
        cv2 = gameState->board[hero.top + dir.vd + 1][hero.left + dir.hd + 2];
        cv3 = gameState->board[hero.top + dir.vd + 2][hero.left + dir.hd + 1]; 
        cv4 = gameState->board[hero.top + dir.vd + 2][hero.left + dir.hd + 2];
        canMove = (cv1 == BOARD_CELL_EMPTY || cv1 == heroIdx + 1) && (cv2 == BOARD_CELL_EMPTY || cv2 == heroIdx + 1) 
            && (cv3 == BOARD_CELL_EMPTY || cv3 == heroIdx + 1) && (cv4 == BOARD_CELL_EMPTY || cv4 == heroIdx + 1);
        break;
    default:
        canMove = false;
        break;
    }

    return canMove;
}

void ClearPosition(HRD_GAME_STATE* gameState, WARRIOR_TYPE type, int left, int top)
{
    switch(type)
    {
    case HT_BLOCK:
        gameState->board[top + 1][left + 1] = BOARD_CELL_EMPTY;
        break;
    case HT_VBAR:
        gameState->board[top + 1][left + 1] = BOARD_CELL_EMPTY;
        gameState->board[top + 2][left + 1] = BOARD_CELL_EMPTY;
        break;
    case HT_HBAR:
        gameState->board[top + 1][left + 1] = BOARD_CELL_EMPTY;
        gameState->board[top + 1][left + 2] = BOARD_CELL_EMPTY;
        break;
    case HT_BOX:
        gameState->board[top + 1][left + 1] = BOARD_CELL_EMPTY;
        gameState->board[top + 1][left + 2] = BOARD_CELL_EMPTY;
        gameState->board[top + 2][left + 1] = BOARD_CELL_EMPTY;
        gameState->board[top + 2][left + 2] = BOARD_CELL_EMPTY;
        break;
    default:
        break;
    }
}

void TakePosition(HRD_GAME_STATE* gameState, int heroIdx, WARRIOR_TYPE type, int left, int top)
{
    switch(type)
    {
    case HT_BLOCK:
        gameState->board[top + 1][left + 1] = heroIdx + 1;
        break;
    case HT_VBAR:
        gameState->board[top + 1][left + 1] = heroIdx + 1;
        gameState->board[top + 2][left + 1] = heroIdx + 1;
        break;
    case HT_HBAR:
        gameState->board[top + 1][left + 1] = heroIdx + 1;
        gameState->board[top + 1][left + 2] = heroIdx + 1;
        break;
    case HT_BOX:
        gameState->board[top + 1][left + 1] = heroIdx + 1;
        gameState->board[top + 1][left + 2] = heroIdx + 1;
        gameState->board[top + 2][left + 1] = heroIdx + 1;
        gameState->board[top + 2][left + 2] = heroIdx + 1;
        break;
    default:
        break;
    }
}

bool AddGameStateHero(HRD_GAME_STATE* gameState, int heroIdx, const WARRIOR& hero)
{
    if(IsPositionAvailable(gameState, hero.type, hero.left, hero.top))
    {
        TakePosition(gameState, heroIdx, hero.type, hero.left, hero.top);
        gameState->heroes.push_back(hero);
        return true;
    }

    return false;
}

void InitGameStateBoard(HRD_GAME_STATE* gameState)
{
    memset(gameState->board, BOARD_CELL_EMPTY, HRD_BOARD_WIDTH * HRD_BOARD_HEIGHT * sizeof(char));

    int i;
    for(i = 0; i < HRD_BOARD_WIDTH; i++)
    {
        gameState->board[0][i] = BOARD_CELL_BORDER;
        gameState->board[HRD_BOARD_HEIGHT - 1][i] = BOARD_CELL_BORDER;
    }
    for(i = 1; i < HRD_BOARD_HEIGHT - 1; i++)
    {
        gameState->board[i][0] = BOARD_CELL_BORDER;
        gameState->board[i][HRD_BOARD_WIDTH - 1] = BOARD_CELL_BORDER;
    }
}

bool InitHdrGameState(HRD_GAME_STATE* gameState, int heroCount, int heroInfo[MAX_HERO_COUNT * 3])
{
    InitGameStateBoard(gameState);

    gameState->parent = NULL;
    gameState->step = 0;
    gameState->move.heroIdx = 0;
    gameState->move.dirIdx = 0;
    for(int i = 0; i < heroCount; i++)
    {
        WARRIOR hero = { (WARRIOR_TYPE)heroInfo[i * 3], heroInfo[i * 3 + 1], heroInfo[i * 3 + 2] };
        if(!AddGameStateHero(gameState, i, hero))
        {
            return false;
        }
    }
    return true;
}

bool InitHrdGame(HRD_GAME& game, START_POSITION& start)
{
    assert(game.heroNames.empty());
    assert(game.states.empty());
    assert(game.zhash.empty());

    game.result = 0;
    game.gameName = start.startName;
    for(int i = 0; i < start.heroCount; i++)
    {
        game.heroNames.push_back(start.heroName[i]);
    }
    game.caoIdx = start.caoIdx;
    HRD_GAME_STATE *gameState = new HRD_GAME_STATE; // try ... catch at caller
    if(InitHdrGameState(gameState, start.heroCount, start.heroInfo))
    {
        unsigned int l2rHash = GetZobristHash(&zob_hash, gameState);
        game.zhash.insert(l2rHash);
#if NO_LR_MIRROR_ALLOW
        unsigned int r2lHash = GetMirrorZobristHash(&zob_hash, gameState);
        game.zhash.insert(r2lHash);
#endif
        game.states.push_back(gameState);
        return true;
    }
    
    delete gameState;
    return false;
}

void OutputMoveRecords(const HRD_GAME& game, const HRD_GAME_STATE* gameState)
{
    std::cout << "Find Result " << game.result << " total " << gameState->step << " steps" << std::endl;

    HRD_GAME_STATE* state = const_cast<HRD_GAME_STATE*>(gameState);
    while(state != NULL)
    {
        if(state->step > 0)
        {
            MOVE_ACTION& curMove = state->move;
            std::cout << "Step " << state->step << " : ";
            std::cout << game.heroNames[curMove.heroIdx] << " move " << DirectionStringFromIndex(curMove.dirIdx) << std::endl;
        }
        state = state->parent;
    }
}

bool IsEscaped(HRD_GAME& game, HRD_GAME_STATE* gameState)
{
    return (gameState->heroes[game.caoIdx - 1].left == CAO_ESCAPE_LEFT) 
            && (gameState->heroes[game.caoIdx - 1].top == CAO_ESCAPE_TOP);
}

void PrintGameState(HRD_GAME_STATE* gameState)
{
    int i,j;

    std::cout << "Game state hash : " << GetZobristHash(&zob_hash, gameState) << std::endl;
    for(i = 0; i < HRD_BOARD_HEIGHT; i++)
    {
        for(j = 0; j < HRD_BOARD_WIDTH; j++)
        {
            std::cout << std::hex << " " << (int)gameState->board[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

HRD_GAME_STATE* MoveHeroToNewState(HRD_GAME_STATE* gameState, int heroIdx, int dirIdx)
{
    if(CanHeroMove(gameState, heroIdx, dirIdx))
    {
        HRD_GAME_STATE* newState = new HRD_GAME_STATE;
        if(newState != NULL)
        {
            CopyGameState(gameState, newState);
            WARRIOR& hero = newState->heroes[heroIdx];
            const DIRECTION& dir = directions[dirIdx];

            ClearPosition(newState, hero.type, hero.left, hero.top);
            TakePosition(newState, heroIdx, hero.type, hero.left + dir.hd, hero.top + dir.vd);
            hero.left = hero.left + dir.hd;
            hero.top = hero.top + dir.vd;

            newState->step = gameState->step + 1;
            newState->parent = gameState;
            newState->move.heroIdx = heroIdx;
            newState->move.dirIdx = dirIdx;
            return newState;
        }
    }

    return NULL;
}

bool AddNewStatePattern(HRD_GAME& game, HRD_GAME_STATE* gameState)
{
    unsigned int l2rHash = GetZobristHash(&zob_hash, gameState);
    if(game.zhash.find(l2rHash) == game.zhash.end())
    {
#if 0
        std::cout << "generate new game state : " << std::endl;
        PrintGameState(newState);
        //_getch();
#endif
        game.zhash.insert(l2rHash);
#if NO_LR_MIRROR_ALLOW
        unsigned int r2lHash = GetMirrorZobristHash(&zob_hash, gameState);
        game.zhash.insert(r2lHash);
#endif
        game.states.push_back(gameState);

        return true;
    }

    return false;
}

void TryHeroContinueMove(HRD_GAME& game, HRD_GAME_STATE* gameState, int heroIdx, int lastDirIdx)
{
    int d = 0;
    /*向四个方向试探移动*/
    for(d = 0; d < MAX_MOVE_DIRECTION; d++)
    {
        if(!IsReverseDirection(d, lastDirIdx)) /*不向原方向移动*/
        {
            HRD_GAME_STATE *newState = MoveHeroToNewState(gameState, heroIdx, d);
            if(newState != NULL)
            {
                if(AddNewStatePattern(game, newState))
                {
                    newState->step--;
                }
                else
                    delete newState;

                return; /*连续移动只能朝一个方向移动一次*/
            }
        }
    }
}

void TrySearchHeroNewState(HRD_GAME& game, HRD_GAME_STATE* gameState, int heroIdx, int dirIdx)
{
    HRD_GAME_STATE *newState = MoveHeroToNewState(gameState, heroIdx, dirIdx);
    if(newState != NULL)
    {
        if(AddNewStatePattern(game, newState))
        {
             /*尝试连续移动，根据华容道游戏规则，连续的移动也只算一步*/
            TryHeroContinueMove(game, newState, heroIdx, dirIdx);
            return;
        }

        delete newState;
    }
}

void SearchNewGameStates(HRD_GAME& game, HRD_GAME_STATE* gameState)
{
    for(int i = 0; i < static_cast<int>(gameState->heroes.size()); i++)
    {
        for(int j = 0; j < MAX_MOVE_DIRECTION; j++)
        {
            TrySearchHeroNewState(game, gameState, i, j);
        }
    }
}

bool ResolveGame(HRD_GAME& game)
{
    int index = 0;
    while(index < static_cast<int>(game.states.size()))
    {
        HRD_GAME_STATE *gameState = game.states[index];
        if(IsEscaped(game, gameState))
        {
            game.result++;
            OutputMoveRecords(game, gameState);
            _getch();
        }
        else
        {
            SearchNewGameStates(game, gameState);
        }

        index++;
    }

    return (game.result > 0);
}

void ReleseGame(HRD_GAME& game)
{
    while(game.states.size() > 0)
    {
        HRD_GAME_STATE *gameState = game.states.front();
        game.states.pop_front();
        delete gameState;
    }
    game.heroNames.clear();
    game.zhash.clear();
    game.result = 0;
}

unsigned int Random32()
{
    return rand() | ((unsigned int)rand() << 15);
}

void MakeCellState(CELL_STATE *cr)
{
    for(int i = 0; i < MAX_WARRIOR_TYPE; i++)
    {
        cr->value[i] = rand() | ((unsigned int)rand() << 15);
    }
}

void InitZobristHash(ZOBRIST_HASH *zob_hash)
{
    srand((unsigned)time(NULL));

    for(int i = 0; i < HRD_GAME_ROW; i++)
    {
        for(int j = 0; j < HRD_GAME_COL; j++)
        {
            MakeCellState(&zob_hash->key[i][j]);
        }
    }
}


int hstateL[] = { 2,0,0, 4,1,0, 2,3,0, 2,0,2, 3,1,2, 2,3,2, 1,0,4, 1,1,3, 1,2,3, 1,3,4 };
int hstateR[] = { 2,0,0, 4,1,0, 2,3,0, 2,0,2, 3,1,2, 2,3,2, 1,0,4, 1,1,3, 1,2,3, 1,3,4 };

int hstate2L[] = { 4,0,0, 2,2,0, 2,3,0, 3,0,2, 1,2,2, 2,3,2, 2,0,3, 1,1,3, 1,1,4, 1,3,4 };
int hstate2R[] = { 2,0,0, 2,1,0, 4,2,0, 2,0,2, 1,1,2, 3,2,2, 1,2,3, 2,3,3, 1,0,4, 1,2,4 };

void testZobristHash()
{
    HRD_GAME_STATE stateL,stateR;
    InitHdrGameState(&stateL, 10, hstateL);
    InitHdrGameState(&stateR, 10, hstateR);

    unsigned int hashL = GetZobristHash(&zob_hash, &stateL);
    unsigned int hashR = GetMirrorZobristHash(&zob_hash, &stateR);
    assert(hashL == hashR);

    HRD_GAME_STATE state2L,state2R;
    InitHdrGameState(&state2L, 10, hstate2L);
    InitHdrGameState(&state2R, 10, hstate2R);
    unsigned int hash2L = GetZobristHash(&zob_hash, &state2L);
    unsigned int hash2R = GetMirrorZobristHash(&zob_hash, &state2R);
    assert(hash2L == hash2R);
    hash2R = GetZobristHash(&zob_hash, &state2R);
    assert(hash2L != hash2R);
}

int main(int argc, char* argv[])
{
    InitZobristHash(&zob_hash);
    testZobristHash();

    HRD_GAME game;

    //if(InitHrdGame(game, BingFenSanLu))
    //if(InitHrdGame(game, SiHengYiShuSiBing))
    //if(InitHrdGame(game, BiYiHengKong))
    //if(InitHrdGame(game, ZhihuiRuoDing))
    if(InitHrdGame(game, HengDaoLiMa))
    {
        std::cout << "Find result for layout : " << game.gameName << std::endl;
        if(ResolveGame(game))
        {
            std::cout << "Find " << game.result << " result(s) totally!" << std::endl;
        }
        else
        {
            std::cout << "Not find result for this layout!" << std::endl;
        }

        ReleseGame(game);
    }

	return 0;
}
