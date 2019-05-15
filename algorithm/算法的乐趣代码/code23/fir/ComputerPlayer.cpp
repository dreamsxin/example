#include "StdAfx.h"
#include "ComputerPlayer.h"

ComputerPlayer::ComputerPlayer(void)
{
    m_searcher = NULL;
    m_depth = 3;
}

ComputerPlayer::ComputerPlayer(const std::string& name)
{
    m_searcher = NULL;
    m_depth = 3;
    SetPlayerName(name);
}

ComputerPlayer::~ComputerPlayer(void)
{
}

int ComputerPlayer::GetNextPosition()
{
    assert(m_state != NULL);
    assert(m_searcher != NULL);

    int np = m_searcher->SearchBestPlay(*m_state, m_depth);
    int row = (np - BOARD_START) / BOARD_COL;
    int col = (np - BOARD_START) % BOARD_COL;

    char cc = 'A' + col;
    std::cout << "Computer play at [" << row + 1 << " , " << cc << "]" << std::endl;

    return np;
}

Searcher* ComputerPlayer::SetSearcher(Searcher* searcher, int depth)
{
    Searcher* tmp = m_searcher;
    m_searcher = searcher;
    m_depth = depth;
    return tmp;
}
