#include "StdAfx.h"
#include <iostream>
#include <string>
#include <cassert>
#include "GameControl.h"

GameControl::GameControl(void)
{
}

GameControl::~GameControl(void)
{
}

void GameControl::SetPlayer(Player *player, int player_id)
{ 
    player->SetPlayerId(player_id);
    player->SetGameState(&m_gameState);
    m_players[player_id] = player; 
}

Player* GameControl::GetPlayer(int player_id)
{
    std::map<int, Player *>::iterator it = m_players.find(player_id);
    if(it != m_players.end())
    {
        return it->second;
    }

    return NULL;
}

void GameControl::InitGameState(const GameState& state)
{
    m_gameState = state;
}

void GameControl::Run()
{
    while(!m_gameState.IsGameOver())
    {
        int playerId = m_gameState.GetCurrentPlayer();
        Player *currentPlayer = GetPlayer(playerId);
        assert(currentPlayer != NULL);

        int np = currentPlayer->GetNextPosition();
        if(np != -1)
        {
            m_gameState.PutChess(np, playerId);
        }
        m_gameState.PrintGame();
        m_gameState.SwitchPlayer();
    }
    int winner = m_gameState.GetWinner();
    if(winner == PLAYER_NULL)
    {
        std::cout << "GameOver, Draw!" << std::endl;
    }
    else
    {
        Player *winnerPlayer = GetPlayer(winner);
        std::cout << "GameOver, " << winnerPlayer->GetPlayerName() << " Win!" << std::endl;
    }
}
