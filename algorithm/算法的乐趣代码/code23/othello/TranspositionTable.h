#pragma once

const int TT_FLAG_EXACT = 1;
const int TT_FLAG_LOWERBOUND = 2;
const int TT_FLAG_UPPERBOUND = 3;

typedef struct tagTranspositionTblEntry
{
    int flag;
    int depth;
    int value;
}TT_ENTRY;

void InitTranspositionTable();
void ResetTranspositionTable();
bool LookupTranspositionTable(unsigned int hash, TT_ENTRY& ttEntry);
void StoreTranspositionTable(unsigned int hash, TT_ENTRY& ttEntry);
