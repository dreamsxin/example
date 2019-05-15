// ga.cpp : Defines the entry point for the console application.
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


const int OBJ_COUNT = 7;
const int CAPACITY = 150;
const int POPULATION_SIZE = 32;
const int MAX_GENERATIONS = 500;
const double P_XOVER = 0.8;
const double P_MUTATION = 0.15;

int Weight[OBJ_COUNT] = {35,30,60,50,40,10,25};
int Value[OBJ_COUNT] = {10,40,30,50,35,40,30};

typedef struct GAType
{
    int gene[OBJ_COUNT];
    int fitness;
    double rf;
    double cf;
}GATYPE;


void GetRandomGene(int *gene, int count)
{
    for(int i = 0; i < count; i++)
    {
        gene[i] = rand() % 2;
    }
}

void Initialize(GATYPE *pop)
{
    for(int i = 0; i < POPULATION_SIZE; i++)
    {
        GetRandomGene(pop[i].gene, OBJ_COUNT);
        pop[i].fitness = 0;
        pop[i].rf = 0.0;
        pop[i].cf = 0.0;
    }
}

int EnvaluateFitness(GATYPE *pop)
{
    int totalFitness = 0;
    for(int i = 0; i < POPULATION_SIZE; i++)
    {
        int tw = 0;
        pop[i].fitness = 0;
        for(int j = 0; j < OBJ_COUNT; j++)
        {
            if(pop[i].gene[j] == 1)
            {
                tw += Weight[j];
                pop[i].fitness += Value[j];
            }
        }
        if(tw > CAPACITY) /*惩罚性措施*/
        {
            pop[i].fitness = 1;
        }
        totalFitness += pop[i].fitness;
    }

    return totalFitness;
}

int GetBestPopulation(GATYPE *pop, GATYPE *bestGene)
{
    int best = 0;
    for(int i = 0; i < POPULATION_SIZE; i++)
    {
        if(pop[i].fitness > pop[best].fitness)
        {
            best = i;
        }
    }
    *bestGene = pop[best];

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

void Select(int totalFitness, GATYPE *pop)
{
    int i;
    GATYPE newPop[POPULATION_SIZE] = { 0 };
    GATYPE best;
    double lastCf = 0.0;
    //计算个体的选择概率和累积概率
    for(i = 0; i < POPULATION_SIZE; i++)
    {
        pop[i].rf = (double)pop[i].fitness / totalFitness;
        pop[i].cf = lastCf + pop[i].rf;
        lastCf = pop[i].cf;
    }
    
    GetBestPopulation(pop, &best);
    for(i = 0; i < POPULATION_SIZE; i++)
    {
        double p = (double)rand() / (RAND_MAX + 1);
        if(p < pop[0].cf)
        {
            newPop[i] = best;//;//pop[0];//
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
    for(i = 0; i < POPULATION_SIZE; i++)
    {
        pop[i] = (newPop[i].fitness == 1) ? best : newPop[i];
    }
}

void ExchangeOver(GATYPE *pop, int first, int second)
{
    /*对随机个数的基因位进行交换*/
    int ecc = rand() % OBJ_COUNT + 1;
    for(int i = 0; i < ecc; i++) 
    {
        /*每个位置被交换的概率是相等的*/
        int idx = rand() % OBJ_COUNT; 
        int tg = pop[first].gene[idx];
        pop[first].gene[idx] = pop[second].gene[idx];
        pop[second].gene[idx] = tg;
    }
}

void Crossover(GATYPE *pop)
{
    int first = -1;//第一个个体已经选择的标识

    for(int i = 0; i < POPULATION_SIZE; i++)
    {
        double p = (double)rand() / (RAND_MAX + 1);
        if(p < P_XOVER)
        {
            if(first < 0)
            {
                first = i; //选择第一个个体
            }
            else
            {
                ExchangeOver(pop, first, i);
                first = -1;//清除第一个个体的选择标识
            }
        }
    }
}

void ReverseGene(GATYPE *pop, int index)
{
    /*对随机个数的基因位进行变异*/
    int mcc = rand() % OBJ_COUNT + 1;
    for(int i = 0; i < mcc; i++)
    {
        /*每个位置被交换的概率是相等的*/
        int gi = rand() % OBJ_COUNT;
        pop[index].gene[gi] = 1 - pop[index].gene[gi];
    }
}

void Mutation(GATYPE *pop)
{
    for(int i = 0; i < POPULATION_SIZE; i++)
    {
        double p = (double)rand() / (RAND_MAX + 1);
        if(p < P_MUTATION)
        {
            ReverseGene(pop, i);
        }
    }
}

const int TEST_ROUND = 500;

int main(int argc, char* argv[])
{
    srand((unsigned)time(NULL));

    int success = 0;
    GATYPE population[POPULATION_SIZE] = { 0 };
    for(int k = 0; k < TEST_ROUND; k++)
    {
        Initialize(population);

        int totalFitness = EnvaluateFitness(population);
        for(int i = 0; i < MAX_GENERATIONS; i++)
        {
            Select(totalFitness, population);
            Crossover(population);
            Mutation(population);
            totalFitness = EnvaluateFitness(population);
        }
        GATYPE best;
        GetBestPopulation(population, &best);
        if(best.fitness == 170)
            success++;
    }

	return 0;
}

