#ifndef __SPLINE3_FITTING_H__
#define __SPLINE3_FITTING_H__


class SplineFitting
{
public:
    SplineFitting();
    virtual ~SplineFitting();

    void CalcSpline(double *Xi, double *Yi, int n, int boundType, double b1, double b2);
    double GetValue(double x);

protected:
    std::vector<double> m_valXi;
    std::vector<double> m_valYi;
    std::vector<double> m_valMi;
    int m_valN;
    bool m_bCalcCompleted;
};

#endif //__SPLINE3_FITTING_H__