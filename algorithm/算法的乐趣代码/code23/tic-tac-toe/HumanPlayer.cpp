#include "StdAfx.h"
#include <iostream>
#include <cassert>
#include "GameState.h"
#include "HumanPlayer.h"

HumanPlayer::HumanPlayer(void)
{
}

HumanPlayer::HumanPlayer(const std::string& name)
{
    SetPlayerName(name);
}

HumanPlayer::~HumanPlayer(void)
{
}

int HumanPlayer::GetNextPosition()
{
    assert(m_state != NULL);

    int np = 0;
    while(true)
    {
        int row, col;
        std::cout << "Please select your position (row = 1-3,col = 1-3): ";
        std::cin >> row >> col;
        np = (row - 1) * BOARD_COL + (col - 1);
        if( ((np >= 0) && (np < 9))
            && m_state->IsEmptyCell(np) )
        {
            break;
        }
        else
        {
            std::cout << "Invalid position on (" << row << " , " << col << ")" << std::endl;
            char line[64];
            if(std::cin.fail())
            {
                std::cin.clear();
                std::cin.getline(line, 64);
            }
        }
    }

    return np;
}