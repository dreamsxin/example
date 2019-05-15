#include "stdafx.h"
#include "Common.h"
#include "ThomasEquation.h"

ThomasEquation::ThomasEquation(int M) 
{ 
    m_matrixA = NULL;
    m_bVal = NULL;
    Init(M, NULL, NULL); 
}

ThomasEquation::ThomasEquation(int M, double *A, double *b) 
{ 
    m_matrixA = NULL;
    m_bVal = NULL;
    Init(M, A, b); 
}

ThomasEquation::~ThomasEquation()
{
    if(m_matrixA)
    {
        delete[] m_matrixA;
        m_matrixA = NULL;
    }
    if(m_bVal)
    {
        delete[] m_bVal;
        m_bVal = NULL;
    }
}

void ThomasEquation::Init(int M, double *A, double *b)
{
    assert((m_matrixA == NULL) && (m_bVal == NULL));
    
    m_DIM = M;
    m_matrixA = new double[m_DIM * m_DIM];
    if(m_matrixA)
    {
        m_bVal = new double[m_DIM];
        if(m_bVal)
        {
            if(A)
            {
                memcpy(m_matrixA, A, sizeof(double) * m_DIM * m_DIM);
            }
            if(b)
            {
                memcpy(m_bVal, b, sizeof(double) * m_DIM);
            }
        }
        else
        {
            delete[] m_matrixA;
            m_matrixA = NULL;
        }
    }
}

double& ThomasEquation::operator()(int row, int col)
{
    assert((row < m_DIM) && (col < m_DIM));
    
    double (*rowPtr)[] = (double (*)[])m_matrixA;
    return *(m_matrixA + row * m_DIM + col);
}

const double& ThomasEquation::operator()(int row, int col) const
{
    assert((row < m_DIM) && (col < m_DIM));

    return *(m_matrixA + row * m_DIM + col);
}

/*追赶法求对三角矩阵方程组的解*/
bool ThomasEquation::Resolve(std::vector<double>& xValue)
{
    assert(xValue.size() == m_DIM);

    std::vector<double> L(m_DIM);
    std::vector<double> M(m_DIM);
    std::vector<double> U(m_DIM);
    std::vector<double> Y(m_DIM);

    /*消元，追的过程*/
    L[0] = m_matrixA[ARR_INDEX(0, 0, m_DIM)];
    U[0] = m_matrixA[ARR_INDEX(0, 1, m_DIM)] / L[0];
    Y[0] = m_bVal[0] / L[0];
    for(int i = 1; i < m_DIM; i++)
    {
        if(IsPrecisionZero(m_matrixA[ARR_INDEX(i, i, m_DIM)]))
        {
            return false;
        }
        M[i] = m_matrixA[ARR_INDEX(i, i - 1, m_DIM)];
        L[i] = m_matrixA[ARR_INDEX(i, i, m_DIM)] - M[i] * U[i - 1];
        Y[i] = (m_bVal[i] - M[i] * Y[i - 1]) / L[i];
        if((i + 1) < m_DIM)
        {
            U[i] = m_matrixA[ARR_INDEX(i, i + 1, m_DIM)] / L[i];
        }
    }
    /*回代求解，赶的过程*/
    xValue[m_DIM - 1] = Y[m_DIM - 1];
    for(int i = m_DIM - 2; i >= 0; i--)
    {
        xValue[i] = Y[i] - U[i] * xValue[i + 1];
    }

    return true;
}

#ifdef _DEBUG
void ThomasEquation::DebugDump()
{
    for(int i = 0; i < m_DIM; i++)
    {
        for(int j = 0; j < m_DIM; j++)
        {
            TRACE(_T("%8.4f "), m_matrixA[i * m_DIM + j]);
            //std::cout << m_matrixA[i * m_DIM + j] << " ";
        }
        TRACE(_T("| %8.4f \n"), m_bVal[i]);
        //std::cout << "| " << m_bVal[i] << std::endl;
    }
}
#endif



#if 0
    double matrixA[3][3] = { {2.5, 2.3, -5.1}, {5.3, 9.6, 1.5}, {8.1, 1.7, -4.3} };
    double b[3] = {3.7, 3.8, 5.5 };
//    double matrixA[3][3] = { {1, 2, 3}, {0, 1, 2}, {2, 4, 1} };
//    double b[3] = {14, 8, 13 };

    CDoubleArray R(3);
    GuassEquation equation(3, (double *)matrixA, b);

    equation.Resolve(R);

#endif