#pragma once

#include "GameConst.h"
#include "Evaluator.h"

typedef struct tagEMPTY_LIST
{
    int cell;
    tagEMPTY_LIST *pred;
    tagEMPTY_LIST *succ;
}EMPTY_LIST;

typedef struct tagMOVES_LIST
{
    EMPTY_LIST *em;
    int goodness;
}MOVES_LIST;

class GameState
{
public:
    GameState(void);
    GameState(const GameState& state);
    ~GameState(void);

    const GameState& operator=(const GameState& state);

    void SetCurrentPlayer(int player_id) { m_playerId = player_id; };
    int GetCurrentPlayer() const { return m_playerId; };
    void PrintGame();
    void InitGameState(int firstPlayer);
    void SetGameState(int *state, int firstPlayer);
    void SetEvaluator(Evaluator *evaluator) { m_evaluator = evaluator; };
    int Evaluate(int max_player_id);
    void SwitchPlayer();
    bool IsGameOver();
    int GetWinner();

    int PutChess(int cell, int player_id);
    bool IsValidPosition(int cell, int player_id);
    bool TestMoves(int player_id);
    int DoPutChess(EMPTY_LIST *em, int player_id, std::vector<int>& flips);
    void UndoPutChess(EMPTY_LIST *em, int player_id, std::vector<int>& flips);
    int FindMoves(int player_id, int opp_player_id, std::vector<MOVES_LIST>& moves);
    int CountMobility(int player_id);
    int CountPosValue(int player_id);
    int CountCell(int player_id);
    int CountEmptyCells();
    unsigned int GetZobristHash();

protected:
    CellType GetCellType(int player_id);
    void InitEmptyList();
    EMPTY_LIST *FindEmptyListByCell(int cell);


    void SingleDirFlips(int cell, int dir, int player_id, int opp_player_id, std::vector<int>& flips);
    int DoFlips(int cell, int player_id, int opp_player_id, std::vector<int>& flips);
    void UndoFlips(std::vector<int>& flips, int opp_player_id);
    bool CanSingleDirFlips(int cell, int dir, int player_id, int opp_player_id);
    bool CanFlips(int cell, int player_id, int opp_player_id);


protected:
    EMPTY_LIST m_Empty_s[64];
    EMPTY_LIST m_EmHead;
    Evaluator *m_evaluator;
    int m_playerId;
    int m_board[BOARD_CELLS];
};

