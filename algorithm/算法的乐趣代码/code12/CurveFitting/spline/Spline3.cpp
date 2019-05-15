#include "stdafx.h"
#include "Common.h"
#include "ThomasEquation.h"
#include "Spline3.h"

SplineFitting::SplineFitting()
{
    m_valN = 0;
    m_bCalcCompleted = false;
}

SplineFitting::~SplineFitting()
{
}

void SplineFitting::CalcSpline(double *Xi, double *Yi, int n, int boundType, double b1, double b2)
{
    assert((boundType == 1) || (boundType == 2));

    double *matrixA = new double[n * n];
    if(matrixA == NULL)
    {
        return;
    }
    double *d = new double[n];
    if(d == NULL)
    {
        delete[] matrixA;
        return;
    }

    m_valN = n;
    m_valXi.assign(Xi, Xi + m_valN);
    m_valYi.assign(Yi, Yi + m_valN);
    m_valMi.resize(m_valN);
    memset(matrixA, 0, sizeof(double) * n * n);

    matrixA[ARR_INDEX(0, 0, m_valN)] = 2.0;
    matrixA[ARR_INDEX(m_valN - 1, m_valN - 1, m_valN)] = 2.0;
    if(boundType == 1) /*第一类边界条件*/
    {
        matrixA[ARR_INDEX(0, 1, m_valN)] = 1.0; //v0
        matrixA[ARR_INDEX(m_valN - 1, m_valN - 2, m_valN)] = 1.0; //un
        double h0 = Xi[1] - Xi[0];
        d[0] = 6 * ((Yi[1] - Yi[0]) / h0 - b1) / h0; //d0
        double hn_1 = Xi[m_valN - 1] - Xi[m_valN - 2];
        d[m_valN - 1] = 6 * (b2 - (Yi[m_valN - 1] - Yi[m_valN - 2]) / hn_1) / hn_1; //dn
    }
    else /*第二类边界条件*/
    {
        matrixA[ARR_INDEX(0, 1, m_valN)] = 0.0; //v0
        matrixA[ARR_INDEX(m_valN - 1, m_valN - 2, m_valN)] = 0.0; //un
        d[0] = 2 * b1; //d0
        d[m_valN - 1] = 2 * b2; //dn
    }
    /*计算ui,vi,di，i = 2,3,...,n-1*/
    for(int i = 1; i < (m_valN - 1); i++)
    {
        double hi_1 = Xi[i] - Xi[i - 1];
        double hi = Xi[i + 1] - Xi[i];
        matrixA[ARR_INDEX(i, i - 1, m_valN)] = hi_1 / (hi_1 + hi); //ui
        matrixA[ARR_INDEX(i, i, m_valN)] = 2.0; 
        matrixA[ARR_INDEX(i, i + 1, m_valN)] = 1 - matrixA[ARR_INDEX(i, i - 1, m_valN)]; //vi = 1 - ui
        d[i] = 6 * ((Yi[i + 1] - Yi[i]) / hi - (Yi[i] - Yi[i - 1]) / hi_1) / (hi_1 + hi); //di
    }

    ThomasEquation equation(m_valN, matrixA, d);
    equation.Resolve(m_valMi);
    m_bCalcCompleted = true;
    
    delete[] matrixA;
    delete[] d;
}

double SplineFitting::GetValue(double x)
{
    if(!m_bCalcCompleted)
    {
        return 0.0;
    }
    if((x < m_valXi[0]) || (x > m_valXi[m_valN - 1]))
    {
        return 0.0;
    }
    int i = 0;
    for(i = 0; i < (m_valN - 1); i++)
    {
        if((x >= m_valXi[i]) && (x < m_valXi[i + 1]))
            break;
    }
    double hi = m_valXi[i + 1] - m_valXi[i];
    double xi_1 = m_valXi[i + 1] - x;
    double xi = x - m_valXi[i];

    double y = xi_1 * xi_1 * xi_1 * m_valMi[i] / (6 * hi);
    y += (xi * xi * xi * m_valMi[i + 1] / (6 * hi));
#if 0
    y += ((m_valYi[i] - m_valMi[i] * hi * hi / 6) * xi_1 / hi);
    y += ((m_valYi[i + 1] - m_valMi[i + 1] * hi * hi / 6) * xi / hi);
#endif

    double Ai = (m_valYi[i + 1] - m_valYi[i]) / hi - (m_valMi[i + 1] - m_valMi[i]) * hi / 6.0;
    y += Ai * x;
    double Bi = m_valYi[i + 1] - m_valMi[i + 1] * hi * hi / 6.0 - Ai * m_valXi[i + 1];
    y += Bi;
    return y;
}
