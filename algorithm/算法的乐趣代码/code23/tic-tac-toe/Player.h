#pragma once

#include <string>

class GameState;

class Player
{
protected:
    Player(void)
    {
    }

public:
    virtual ~Player(void)
    {
    }

    virtual int GetNextPosition() = NULL; 
    std::string GetPlayerName() { return m_playerName; };
    void SetPlayerName(const std::string& name) { m_playerName = name; };
    int GetPlayerId() { return m_playerId; };
    void SetPlayerId(int id) { m_playerId = id; };
    GameState* GetGameState() { return m_state; };
    void SetGameState(GameState* state) { m_state = state; };

protected:
    int m_playerId;
    std::string m_playerName;
    GameState *m_state;
};

