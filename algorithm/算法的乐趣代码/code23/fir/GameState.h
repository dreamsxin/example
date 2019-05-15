#pragma once

#include "GameConst.h"
#include "Evaluator.h"
#include "EvaluatorData.h"
#include "Support.h"

typedef struct tagMOVES_LIST
{
    int cell;
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

    bool PutChess(int cell, int player_id);
    bool IsValidPosition(int cell, int player_id);
    int FindMoves(int player_id, std::vector<MOVES_LIST>& moves);
    int CountPosValue(int player_id);
    unsigned int GetHash();
	int SearchPatterns(EvaluatorData &ev_ata);
    void DoPutChess(int cell, int player_id);
    void UndoPutChess(int cell);

protected:
    CellType GetCellType(int player_id);

    int CountEmptyCell();
    int AnalysisLine(int st, int dir_inc, EvaluatorData &ev_ata);
    bool CheckFiveInRow(int cell, int player_id);
    bool CheckLinefive(int cell, int dir_inc, int player_id);
    unsigned int CalcZobristHash();
    bool IsForbidden(int cell, int player_id);
    bool IsMatchSingleForbidden(FORBIDDEN_ITEM& item, int cell, int player_id);
    int SkipEmptyCell(int cs, int dir_inc);
    int SearchAndCountChess(int cs, int dir_inc, int chess_id, int& count);

protected:
    Evaluator *m_evaluator;
    int m_playerId;
    int m_board[BOARD_CELLS];
    bool m_gameOver;
    int m_winnerId;
    unsigned int m_hash;
};

