#include "stdafx.h"
#include "Common.h"
#include "GuassEquation.h"

GuassEquation::GuassEquation(int M) 
{ 
    m_matrixA = NULL;
    m_bVal = NULL;
    Init(M, NULL, NULL); 
}

GuassEquation::GuassEquation(int M, double *A, double *b) 
{ 
    m_matrixA = NULL;
    m_bVal = NULL;
    Init(M, A, b); 
}

GuassEquation::~GuassEquation()
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

void GuassEquation::Init(int M, double *A, double *b)
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

double& GuassEquation::operator()(int row, int col)
{
    assert((row < m_DIM) && (col < m_DIM));
    
    double (*rowPtr)[] = (double (*)[])m_matrixA;
    return *(m_matrixA + row * m_DIM + col);
}

const double& GuassEquation::operator()(int row, int col) const
{
    assert((row < m_DIM) && (col < m_DIM));

    return *(m_matrixA + row * m_DIM + col);
}

/*带列主元的高斯消去法解方程组，最后的解在matrixA的对角线上*/
bool GuassEquation::Resolve(std::vector<double>& xValue)
{
    assert(xValue.size() == m_DIM);

    /*消元，得到上三角阵*/
    for(int i = 0; i < m_DIM - 1; i++)
    {
        /*按列选主元*/
        int pivotRow = SelectPivotalElement(i);
        if(pivotRow != i)/*如果有必要，交换行*/
        {
            SwapRow(i, pivotRow);
        }
        if(IsPrecisionZero(m_matrixA[i * m_DIM + i])) /*主元是0? 不存在唯一解*/
        {
            return false;
        }
        /*对系数归一化处理，使行第一个系数是1.0*/
        SimplePivotalRow(i, i);
        /*逐行进行消元*/
        for(int j = i + 1; j < m_DIM; j++)
        {
            RowElimination(i, j, i);
        }
    }
    /*回代求解*/
    m_matrixA[(m_DIM - 1) * m_DIM + m_DIM - 1] = m_bVal[m_DIM - 1] / m_matrixA[(m_DIM - 1) * m_DIM + m_DIM - 1];
    for(int i = m_DIM - 2; i >= 0; i--)
    {
        double totalCof = 0.0;
        for(int j = i + 1; j < m_DIM; j++)
        {
            totalCof += m_matrixA[i * m_DIM + j] * m_matrixA[j * m_DIM + j];
        }
        m_matrixA[i * m_DIM + i] = (m_bVal[i] - totalCof) / m_matrixA[i * m_DIM + i];
    }

    /*将对角线元素的解逐个存入解向量*/
    for(int i = 0; i < m_DIM; i++)
    {
        xValue[i] = m_matrixA[i * m_DIM + i];
    }

    return true;
}

#ifdef _DEBUG
void GuassEquation::DebugDump()
{
    for(int i = 0; i < m_DIM; i++)
    {
        for(int j = 0; j < m_DIM; j++)
        {
            std::cout << m_matrixA[i * m_DIM + j] << " ";
        }
        std::cout << "| " << m_bVal[i] << std::endl;
    }
}
#endif

/*M是维度，column取值范围从0到M-1*/
int GuassEquation::SelectPivotalElement(int column)
{
    assert(column < m_DIM);
    
    int row = column;
    double maxU = std::fabs(m_matrixA[column * m_DIM + column]);

    for(int i = column + 1; i < m_DIM; i++)
    {
        if(std::fabs(m_matrixA[i * m_DIM + column]) > maxU)
        {
            maxU = std::fabs(m_matrixA[i * m_DIM + column]);
            row = i;
        }
    }

    return row;
}

void GuassEquation::SwapRow(int row1, int row2)
{
    assert((row1 < m_DIM) && (row2 < m_DIM));
    
    double temp;
    for(int i = 0; i < m_DIM; i++)
    {
        temp = m_matrixA[row1 * m_DIM + i];
        m_matrixA[row1 * m_DIM + i] = m_matrixA[row2 * m_DIM + i];
        m_matrixA[row2 * m_DIM + i] = temp;
    }
    temp = m_bVal[row1];
    m_bVal[row1] = m_bVal[row2];
    m_bVal[row2] = temp;
}

void GuassEquation::SimplePivotalRow(int row, int startColumn)
{
    assert((row < m_DIM) && (startColumn < m_DIM));

    double simple = m_matrixA[row * m_DIM + startColumn];

    for(int i = startColumn; i < m_DIM; i++)
    {
        m_matrixA[row * m_DIM + i] /= simple;
    }
    m_bVal[row] /= simple;
}

void GuassEquation::RowElimination(int rowS, int rowE, int startColumn)
{
    assert((rowS < m_DIM) && (rowE < m_DIM) && (startColumn < m_DIM));

    double simple = m_matrixA[rowE * m_DIM + startColumn];

    for(int i = startColumn; i < m_DIM; i++)
    {
        m_matrixA[rowE * m_DIM + i] -= m_matrixA[rowS * m_DIM + i] * simple;
    }
    m_bVal[rowE] -= m_bVal[rowS] * simple;
}

#if 0
    double matrixA[3][3] = { {2.5, 2.3, -5.1}, {5.3, 9.6, 1.5}, {8.1, 1.7, -4.3} };
    double b[3] = {3.7, 3.8, 5.5 };
//    double matrixA[3][3] = { {1, 2, 3}, {0, 1, 2}, {2, 4, 1} };
//    double b[3] = {14, 8, 13 };

    CDoubleArray R(3);
    GuassEquation equation(3, (double *)matrixA, b);

    equation.Resolve(R);

#endif