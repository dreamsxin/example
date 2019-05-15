#ifndef __THOMAS_EQUATION_H__
#define __THOMAS_EQUATION_H__


class ThomasEquation
{
public:
    ThomasEquation(int M);
    ThomasEquation(int M, double *A, double *b);
    virtual ~ThomasEquation();

    void Init(int M, double *A, double *b);
    double& operator()(int row, int col);
    const double& operator()(int row, int col) const;

    bool Resolve(std::vector<double>& xValue);
#ifdef _DEBUG
    void DebugDump();
#endif

protected:
    double *m_matrixA;
    int m_DIM;
    double *m_bVal;
};

#endif //__THOMAS_EQUATION_H__