#pragma once
#include <string>
#include <iostream>
#include "player.h"

class HumanPlayer : public Player
{
public:
    HumanPlayer(void);
    HumanPlayer(const std::string& name);
    virtual ~HumanPlayer(void);

    virtual int GetNextPosition(); 
};
