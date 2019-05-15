#include "StdAfx.h"
#include "ZobristHash.h"
#include "GameState.h"
#include "TranspositionTable.h"

static std::map<unsigned int, TT_ENTRY> tt_map;

void InitTranspositionTable()
{
    InitZobristHashTbl();

    tt_map.erase(tt_map.begin(), tt_map.end());
}

void ResetTranspositionTable()
{
    tt_map.erase(tt_map.begin(), tt_map.end());
}

bool LookupTranspositionTable(unsigned int hash, TT_ENTRY& ttEntry)
{
    std::map<unsigned int, TT_ENTRY>::iterator it = tt_map.find(hash);
    if(it != tt_map.end())
    {
        ttEntry = it->second;
        return true;
    }

    return false;
}

void StoreTranspositionTable(unsigned int hash, TT_ENTRY& ttEntry)
{
    std::map<unsigned int, TT_ENTRY>::iterator it = tt_map.find(hash);
    if(it != tt_map.end())
    {
        TT_ENTRY& old_entry = it->second;
        if(ttEntry.depth >= old_entry.depth)
        {
            old_entry = ttEntry;
        }
    }
    else
    {
        tt_map[hash] = ttEntry;
    }
}
