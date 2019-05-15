// ga2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cassert>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <vector>
#include <cassert>
//#include <functional>
#include <numeric>
#include <algorithm>
#include <cmath>


double BinFunc(double x)
{
    return (-2.0*x*x + x + 15);
}
/*
y=ax^2 + bx + c
x=-b/2a=0.25
y=(4ac-b^2)/4a=15.125
*/

//[-2.5, 3.0]

/*

unsigned int GrayEncoding(unsigned int decimal)
{
    return decimal ^ (decimal >> 1);
}

unsigned int DecimalEncoding(unsigned int gray)
{
    gray ^= gray >> 16;
    gray ^= gray >> 8;
    gray ^= gray >> 4;
    gray ^= gray >> 2;
    gray ^= gray >> 1;
    //gray ^= gray ^ 1;

    return gray;
}

unsigned int DecimalEncoding(unsigned int gray)
{
    unsigned int decimal = gray;
    while(gray >>= 1)
        decimal ^= gray;

    return decimal;
}

*/

unsigned short GrayEncoding(unsigned short decimal)
{
    return decimal ^ (decimal >> 1);
}

unsigned short DecimalEncoding(unsigned short gray)
{
    unsigned short decimal = gray;
    while(gray >>= 1)
        decimal ^= gray;

    return decimal;
}

const int POPULATION_SIZE = 32;
const int MAX_GENERATIONS = 1000;
const double P_XOVER = 0.8;
const double P_MUTATION = 0.15;


typedef struct GAType
{
    unsigned short gene;
    double fitness;
    double rf;
    double cf;
    double low;
    double high;
}GATYPE;


unsigned short GetRandomGene(double low, double high)
{
    //产生随机数u，low <= u <= high
    double u = (double)rand() / (RAND_MAX + 1) * (high - low) + low;
    //映射到正整数0-65535范围上
    unsigned short k = (unsigned short)((u - low) / (high - low) * 65535);
    //转换为格雷码
    unsigned short g = GrayEncoding(k);

    return g;
}

double EnvaluateFitness(GATYPE *pop)
{
    double totalFitness = 0.0;
    for(int i = 0; i < POPULATION_SIZE; i++)
    {
        unsigned short k = DecimalEncoding(pop[i].gene);
        double u = k * (pop[i].high - pop[i].low) / 65535 + pop[i].low;

        pop[i].fitness = BinFunc(u);
        totalFitness += pop[i].fitness;
    }

    return totalFitness;
}

int GetBestPopulation(GATYPE *pop)
{
    int best = 0;
    for(int i = 0; i < POPULATION_SIZE; i++)
    {
        if(pop[i].fitness > pop[best].fitness)
        {
            best = i;
        }
    }

    return best;
}

int GetWorstPopulation(GATYPE *pop)
{
    int worst = 0;
    for(int i = 0; i < POPULATION_SIZE; i++)
    {
        if(pop[i].fitness < pop[worst].fitness)
        {
            worst = i;
        }
    }

    return worst;
}

void Select(double totalFitness, GATYPE *pop)
{
    int i;
    GATYPE newPop[POPULATION_SIZE] = { 0 };
    double lastCf = 0.0;
    for(i = 0; i < POPULATION_SIZE; i++)
    {
        pop[i].rf = pop[i].fitness / totalFitness;
        pop[i].cf = lastCf + pop[i].rf;
        lastCf = pop[i].cf;
    }
    
    int best = GetBestPopulation(pop);
    for(i = 0; i < POPULATION_SIZE; i++)
    {
        double p = (double)rand() / (RAND_MAX + 1);
        if(p < pop[0].cf)
        {
            newPop[i] = pop[best];
        }
        else
        {
            for(int j = 0; j < POPULATION_SIZE; j++)
            {
                if((p >= pop[j].cf) && (p < pop[j + 1].cf))
                {
                    newPop[i] = pop[j + 1];
                }
            }
        }
    }
#if 1
    bool replaceWorst = false;
    unsigned short bestGene = pop[best].gene;
    int newBest = GetBestPopulation(newPop);
    if(pop[best].fitness > newPop[newBest].fitness)
    {
        replaceWorst = true;
    }
#endif    
    for(i = 0; i < POPULATION_SIZE; i++)
    {
        pop[i] = newPop[i];
    }
#if 1
    if(replaceWorst)
    {
        int newWorst = GetWorstPopulation(pop);
        pop[newWorst].gene = bestGene;
    }
#endif
}

unsigned short MakeMask(int sBit, int eBit)
{
    unsigned short mask = 0xFFFF;
    int keep = (eBit - sBit + 1);

    mask >>= (16 - keep);
    mask <<= (16 - eBit);

    return mask;
}

void ExchangeOver(GATYPE *pop, int first, int second)
{
    int ps = (int)((double)rand() / (RAND_MAX + 1) * 6 + 0.5);
    int pe = (int)((double)rand() / (RAND_MAX + 1) * 6 + 8.5);

    unsigned short mask = MakeMask(ps, pe);
    unsigned short revMask = ~mask;
    unsigned short temp1 = pop[first].gene & mask;
    unsigned short temp2 = pop[second].gene & mask;

    pop[first].gene &= revMask;
    pop[first].gene |= temp2;
    pop[second].gene &= revMask;
    pop[second].gene |= temp1;
}

void Crossover(GATYPE *pop)
{
    int first = -1;

    for(int i = 0; i < POPULATION_SIZE; i++)
    {
        double p = (double)rand() / (RAND_MAX + 1);
        if(p < P_XOVER)
        {
            if(first < 0)
            {
                first = i;
            }
            else
            {
                ExchangeOver(pop, first, i);
                first = -1;
            }
        }
    }
}

void ReverseBit(GATYPE *pop, int index, int bit)
{
    unsigned short mask = 0x0001 << (15 - bit);

    if(pop[index].gene & mask)
    {
        pop[index].gene &= ~mask;
    }
    else
    {
        pop[index].gene |= mask;
    }
}
/*
void Mutation(GATYPE *pop)
{
    for(int i = 0; i < POPSIZE; i++)
    {
        for(int j = 0; j < 16; j++)
        {
            double p = (double)rand() / (RAND_MAX + 1);
            if(p < P_MUTATION)
            {
                ReverseBit(pop, i, j);
            }
        }
    }
}
*/

void Mutation(GATYPE *pop)
{
    for(int i = 0; i < POPULATION_SIZE; i++)
    {
        double p = (double)rand() / (RAND_MAX + 1);
        if(p < P_MUTATION)
        {
            int bit = (int)((double)rand() / (RAND_MAX + 1) * 15);
            ReverseBit(pop, i, bit);
        }
    }
}

void Initialize(GATYPE *pop)
{
    for(int i = 0; i < POPULATION_SIZE; i++)
    {
        pop[i].low = -2.5;
        pop[i].high = 3.0;
        pop[i].gene = GetRandomGene(pop[i].low, pop[i].high);
        pop[i].fitness = 0.0;
        pop[i].rf = 0.0;
        pop[i].cf = 0.0;
    }
}

double DiffSquare(double value, double avg)
{
    return (value - avg) * (value - avg);
}

double CalcStandardDeviation(double *params, int count)
{
    assert(count > 0);

    double average = std::accumulate(params, params + count, 0.0) / count;
    std::vector<double> diffSq(count);
    std::transform(params, params + count, diffSq.begin(), 
                   std::bind2nd(std::ptr_fun(DiffSquare), average));
    
    double samvar = std::accumulate(diffSq.begin(), diffSq.end(), 0.0) / count;
    
    return std::sqrt(samvar);
}

const int TEST_ROUND = 500;

int main(int argc, char* argv[])
{
    srand((unsigned)time(NULL));

    GATYPE population[POPULATION_SIZE] = { 0 };
    double fitness[TEST_ROUND] = { 0.0 };
    for(int k = 0; k < TEST_ROUND; k++)
    {
        Initialize(population);

        for(int i = 0; i < MAX_GENERATIONS; i++)
        {
            double totalFitness = EnvaluateFitness(population);
            Select(totalFitness, population);
            Crossover(population);
            Mutation(population);
        }
        int best = GetBestPopulation(population);
        fitness[k] = population[best].fitness;
    }
    double sdv = CalcStandardDeviation(fitness, TEST_ROUND);
    double avgfitness = std::accumulate(fitness, fitness + TEST_ROUND, 0.0) / TEST_ROUND;


	return 0;
}

