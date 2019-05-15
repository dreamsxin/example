#ifndef __GUASS_EQUATION_H__
#define __GUASS_EQUATION_H__


class GuassEquation
{
public:
    GuassEquation(int M);
    GuassEquation(int M, double *A, double *b);
    virtual ~GuassEquation();

    void Init(int M, double *A, double *b);
    double& operator()(int row, int col);
    const double& operator()(int row, int col) const;

    bool Resolve(std::vector<double>& xValue);
#ifdef _DEBUG
    void DebugDump();
#endif
protected:
    int SelectPivotalElement(int column);
    void SwapRow(int row1, int row2);
    void SimplePivotalRow(int row, int startColumn);
    void RowElimination(int rowS, int rowE, int startColumn);

protected:
    double *m_matrixA;
    int m_DIM;
    double *m_bVal;
};

#endif //__GUASS_EQUATION_H__