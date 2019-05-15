// LeastSquare.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GuassEquation.h"
#include "Common.h"

//最小二乘法曲线拟合
//typedef CArray<double,double>CDoubleArray;
typedef std::vector<double> CDoubleArray;
bool CalculateCurveParameter(CDoubleArray *X,CDoubleArray *Y,long M,long N,CDoubleArray *A)
{
    //X,Y --  X,Y两轴的坐标
    //M   --  结果变量组数
    //N   --  采样数目
    //A   --  结果参数

    long i,j,k;
    double Z,D1,D2,C,P,G,Q;
    CDoubleArray B,T,S;
    B.resize(N);
    T.resize(N);
    S.resize(N);
    if(M>N)M=N;
    for(i=0;i<M;i++)
        (*A)[i]=0;
    Z=0;
    B[0]=1;
    D1=N;
    P=0;
    C=0;
    for(i=0;i<N;i++)
    {
        P=P+(*X)[i]-Z;
        C=C+(*Y)[i];
    }
    C=C/D1;
    P=P/D1;
    (*A)[0]=C*B[0];
    if(M>1)
    {
        T[1]=1;
        T[0]=-P;
        D2=0;
        C=0;
        G=0;
        for(i=0;i<N;i++)
        {
            Q=(*X)[i]-Z-P;
            D2=D2+Q*Q;
            C=(*Y)[i]*Q+C;
            G=((*X)[i]-Z)*Q*Q+G;
        }
        C=C/D2;
        P=G/D2;
        Q=D2/D1;
        D1=D2;
        (*A)[1]=C*T[1];
        (*A)[0]=C*T[0]+(*A)[0];
    }
    for(j=2;j<M;j++)
    {
        S[j]=T[j-1];
        S[j-1]=-P*T[j-1]+T[j-2];
        if(j>=3)
        {
            for(k=j-2;k>=1;k--)
                S[k]=-P*T[k]+T[k-1]-Q*B[k];
        }
        S[0]=-P*T[0]-Q*B[0];
        D2=0;
        C=0;
        G=0;
        for(i=0;i<N;i++)
        {
            Q=S[j];
            for(k=j-1;k>=0;k--)
                Q=Q*((*X)[i]-Z)+S[k];
            D2=D2+Q*Q;
            C=(*Y)[i]*Q+C;
            G=((*X)[i]-Z)*Q*Q+G;
        }
        C=C/D2;
        P=G/D2;
        Q=D2/D1;
        D1=D2;
        (*A)[j]=C*S[j];
        T[j]=S[j];
        for(k=j-1;k>=0;k--)
        {
            (*A)[k]=C*S[k]+(*A)[k];
            B[k]=T[k];
            T[k]=S[k];
        }
    }
    return true;
}

bool LeastSquare(const std::vector<double>& x_value, const std::vector<double>& y_value, 
                 int M, std::vector<double>& a_value)
{
    assert(x_value.size() == y_value.size());
    assert(a_value.size() == M);

    double *matrix = new double[M * M];
    double *b= new double[M];

    std::vector<double> x_m(x_value.size(), 1.0);
    std::vector<double> y_i(y_value.size(), 0.0);
    for(int i = 0; i < M; i++)
    {
        matrix[ARR_INDEX(0, i, M)] = std::accumulate(x_m.begin(), x_m.end(), 0.0);
        for(int j = 0; j < static_cast<int>(y_value.size()); j++)
        {
            y_i[j] = x_m[j] * y_value[j];
        }
        b[i] = std::accumulate(y_i.begin(), y_i.end(), 0.0);
        for(int k = 0; k < static_cast<int>(x_m.size()); k++)
        {
            x_m[k] *= x_value[k];
        }
    }
    for(int row = 1; row < M; row++)
    {
        for(int i = 0; i < M - 1; i++)
        {
            matrix[ARR_INDEX(row, i, M)] = matrix[ARR_INDEX(row - 1, i + 1, M)];
        }
        matrix[ARR_INDEX(row, M - 1, M)] = std::accumulate(x_m.begin(), x_m.end(), 0.0);
        for(int k = 0; k < static_cast<int>(x_m.size()); k++)
        {
            x_m[k] *= x_value[k];
        }
    }

    GuassEquation equation(M, matrix, b);
    delete[] matrix;
    delete[] b;

    return equation.Resolve(a_value);
}
const int N = 12;
double x_value[N] = { 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
double y_value[N] = { 8.41, 9.94, 11.58, 13.02, 14.33, 15.92, 17.54, 19.22, 20.49, 22.01, 23.53, 24.47 };
/*
const int N = 6;
double x_value[N] = { 0.5, 1.0, 1.5, 2.0, 2.5, 3.0 };
double y_value[N] = { 1.75, 2.45, 3.81, 4.80, 7.00, 8.60 };
*/

int main(int argc, char* argv[])
{
    std::vector<double> X,Y;
    std::vector<double> A(2);

    X.assign(x_value, x_value + N);
    Y.assign(y_value, y_value + N);

    CalculateCurveParameter(&X, &Y, 2, N, &A);

    CDoubleArray B(2);
    LeastSquare(X, Y, 2, B); 

	return 0;
}

