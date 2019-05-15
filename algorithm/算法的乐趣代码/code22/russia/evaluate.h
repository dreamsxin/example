#ifndef __EVALUATE_H__
#define __EVALUATE_H__

#include "type_def.h"

bool IsFullRowStatus(RUSSIA_GAME *game, int row);

int GetLandingHeight(RUSSIA_GAME *game, B_SHAPE *bs, int row, int col);
int GetErodedPieceCellsMetric(RUSSIA_GAME *game, B_SHAPE *bs, int row, int col);
int GetBoardRowTransitions(RUSSIA_GAME *game, B_SHAPE *bs, int row, int col);
int GetBoardColTransitions(RUSSIA_GAME *game, B_SHAPE *bs, int row, int col);
int GetBoardBuriedHoles(RUSSIA_GAME *game, B_SHAPE *bs, int row, int col);
int GetBoardWells(RUSSIA_GAME *game, B_SHAPE *bs, int row, int col);

int EvaluateFunction(RUSSIA_GAME *game, B_SHAPE *bs, int row, int col);
int PrioritySelection(RUSSIA_GAME *game, int r_index, int row, int col);


#endif //__EVALUATE_H__