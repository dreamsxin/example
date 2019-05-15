#pragma once

#include <string>
#include <iostream>
#include "player.h"
#include "Searcher.h"

class ComputerPlayer : public Player
{
public:
    ComputerPlayer(void);
    ComputerPlayer(const std::string& name);
    virtual ~ComputerPlayer(void);

    virtual int GetNextPosition(); 
    Searcher* SetSearcher(Searcher* searcher, int depth);

protected:
    Searcher* m_searcher;
    int m_depth;
};
