#pragma once

#include "GameState.h"
#include "Searcher.h"
#include "Player.h"
#include <map>


class GameControl
{
public:
    GameControl(void);
    ~GameControl(void);

    void SetPlayer(Player *player, int player_id);
    Player* GetPlayer(int player_id);
    void InitGameState(const GameState& state);
    void Run();

protected:
    GameState m_gameState;
    std::map<int, Player *> m_players;
};
