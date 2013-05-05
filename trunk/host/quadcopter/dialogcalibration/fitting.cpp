//     Copyright (c) 2013 js200300953@qq.com All rights reserved.
//         ========圆点博士微型四轴飞行器配套程序声明========
// 
// 圆点博士微型四轴飞行器配套程序包括上位机程序、下位机Bootloader和
//     下位机App，及它们的源代码，以下总称“程序”。
// 程序由js200300953编写。
// 程序仅为使用者提供参考，js200300953不对程序提供任何明示或暗含的担保，
//     不对在使用该程序中出现的意外或者损失负责，
//     也不对因使用该程序而引起的第三方索赔负责。
// 使用者可以以学习为目的修改和使用该程序，请勿以商业的目的使用该程序。
// 修改该程序时，必须保留原版权声明，并且不能修改原版权声明。
// 
// 更多资料见：
//     http://blog.sina.com.cn/js200300953
//     http://www.etootle.com/
//     http://www.amobbs.com/thread-5504090-1-1.html
//     圆点博士微型四轴飞行器QQ群：276721324

/* Fiting.cpp
 * 2012-06-14 09:09:00
 * js200300953
 */

#include "fitting.h"

void Fitting::reset(void)
{
    for(int i=0;i<MATRIX_SIZE;i++)
    {
        m_result[i] = 0.0;
        for(int j=0;j<MATRIX_SIZE+1;j++)
            m_matrix[i][j] = 0.0;
        //
    }
}

void Fitting::input(double x,double y,double z)
{
    double pn[MATRIX_SIZE];
    pn[0] = x*x;
    pn[1] = y*y;
    pn[2] = z*z;
    pn[3] = x;
    pn[4] = y;
    pn[5] = z;
    pn[6] = 1.0;
    //
    for(int i=0;i<MATRIX_SIZE;i++)
    {
        /* 系数矩阵是对称的，
         * 遍历下三角就足够（包括对角线）。*/
        for(int j=0;j<=i;j++)
        {
            m_matrix[i][j] += pn[i] * pn[j];
        }
    }
}

void Fitting::calculate(double radius)
{
    /*
     * 扩展上三角和最后一列。 */
    for(int i=0;i<MATRIX_SIZE;i++)
    {
        for(int j=i+1;j<MATRIX_SIZE;j++)
        {
            m_matrix[i][j] = m_matrix[j][i];
        }
        m_matrix[i][MATRIX_SIZE] = 0.0;
    }
    /*
     * 解线性方程。 */
    Matrix A;
    A.NewMatrix(MATRIX_SIZE,MATRIX_SIZE+1);// 最后一列是0。
    A.Phase((double *)m_matrix);
    //
    A.RowSimplify();
    /*
     * 作弊，忽略最后一行，解齐次方程的手段，否则只有零解。 */
    A.SetElement(MATRIX_SIZE-1,MATRIX_SIZE-1,0.0);
    A.RowSimplifyMaxly();
    //
    double C[MATRIX_SIZE];
    for(int i=0;i<MATRIX_SIZE;i++)
        C[i] = 1;
    //
    double vp[MATRIX_SIZE+1];
    A.Solve(C,vp);
    A.DeleteMatrix();
    //
    double k = (4*radius*radius);
    k /= (vp[3]*vp[3]/vp[0] + vp[4]*vp[4]/vp[1] \
          + vp[5]*vp[5]/vp[2] - 4*vp[6]);
    m_result[0] = std::sqrt(k * vp[0]);
    m_result[1] = std::sqrt(k * vp[1]);
    m_result[2] = std::sqrt(k * vp[2]);
    m_result[3] = k*vp[3] / (2 * m_result[0]);
    m_result[4] = k*vp[4] / (2 * m_result[1]);
    m_result[5] = k*vp[5] / (2 * m_result[2]);
}

/*
int Fitting_test(void)
{
    Fitting ft;
    //
    ft.input( 2.5, 0.3 , 0.8);
    ft.input(-1.5, 0.3, 0.8);
    ft.input( 0.5, 3.3, 0.8);
    ft.input( 0.5,-2.7, 0.8);
    ft.input( 0.5, 0.3, 4.8);
    ft.input( 0.5, 0.3,-3.2);
    //
    //~ ft.input( 1, 0 , 0);
    //~ ft.input(-1, 0 , 0);
    //~ ft.input( 0, 1 , 0);
    //~ ft.input( 0,-1 , 0);
    //~ ft.input( 0, 0 , 1);
    //~ ft.input( 0, 0 ,-1);
    //
    ft.calculate(1);
    double * rs = ft.getSolution();
    for(int i=0;i<6;i++)
        cout << i << " : " << rs[i] << endl;
    //
    return 0;
}
*/
