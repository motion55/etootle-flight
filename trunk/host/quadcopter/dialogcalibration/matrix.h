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

/* matrix.h
 * 2012-9-4 18:44:18
 * js200300953
 */
#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <stdlib.h>
//~ #include <strstream>
//~ #include <string.h>

using namespace std;

class Matrix
{
public:
    //
    //新建矩阵；
    void NewMatrix(int _n,int _m);
    //释放矩阵；
    void DeleteMatrix();
    //填充0；
    void Zero(void);
    //随机填充；
    void Random(int min,int max,int seed=1);
    //复制；
    Matrix Copy(void);
    //从一个行顺序的数组输入；
    void Phase(double * rowOrderElement);
    //设置某个元素的值；
    void SetElement(int i,int j,double value);
    //从标准输入读矩阵；
    void ReadStd();
    //输出矩阵；
    void Print();
    //行化简，返回交换行的次数；
    int RowSimplify();
    //求前n列的行列式，先要经过行化简，参数swp为行化简的返回值；
    double Det(int swp);
    //求矩阵的秩，要先行化简；
    int Rank();
    //化行最简型，先要经过行化简，返回(任意常量数+1)，例如1表示唯一解；
    int RowSimplifyMaxly(void);
    //解线性方程组，以增广矩阵形式输入，先要化行最简；
    //C为任意常数数组，解出后放在sol；
    void Solve(double * C,double * sol);
    //检查是否正确解；
    bool IsSolution(double * sol);
private:
    //
    //测试方程组是否解正确；
    void TestSolve();
    //测试秩是否计算正确；
    void TestRank();
    //测试行列式是否计算正确；
    void TestDet();
    //
    //取绝对值；
    static inline double Abs(double a)
    {
        return a<0 ? -a : a;
    }
    //
    //判断相等；
    static inline bool Equal(double a,double b)
    {
        return Abs(a-b) < 1e-6;
    }
    //
    //
    double ** a;//矩阵元素，行指针数组；
    int n;//行数；
    int m;//列数；
    const static int MAX_SOL = 32;//未知数最大数量
    //
};


#endif // MATRIX_H
