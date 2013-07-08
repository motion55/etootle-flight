//     Copyright (c) 2013 js200300953@qq.com All rights reserved.
//         ==================================================
//         ========圆点博士微型四轴飞行器配套软件声明========
//         ==================================================
//     圆点博士微型四轴飞行器配套软件包括上位机程序、下位机Bootloader
// 、下位机App和遥控程序，及它们的源代码，以下总称“软件”。
//     软件仅提供参考，js200300953不对软件作任何担保，不对因使用该软件
// 而出现的损失负责。
//     软件可以以学习为目的修改和使用，但不允许以商业的目的使用该软件。
//     修改该软件时，必须保留原版权声明。
// 
//     更多资料见：
// http://blog.sina.com.cn/js200300953
// http://www.etootle.com/
// http://www.eeboard.com/bbs/forum-98-1.html#separatorline
// 圆点博士微型四轴飞行器QQ群：276721324

//Matrix.cpp
//创建时间：2011-4-26 16:16:43
//修改时间：2011-4-28 10:32:01

#include "matrix.h"

//
//新建矩阵；
void Matrix::NewMatrix(int _n,int _m)
{
    n = _n;
    m = _m;
    //
    a = new double* [n];
    for(int i=0;i<n;i++)
        a[i] = new double[m];
    //
}

//
//释放矩阵；
void Matrix::DeleteMatrix()
{
    for(int i=0;i<n;i++)
        delete [] a[i];
    //
}

//
//填充0；
void Matrix::Zero(void)
{
    for(int i=0;i<n;i++)
        for(int j=0;j<m;j++)
            a[i][j] = 0;
        //
    //
}

//
//随机填充；
void Matrix::Random(int min,int max,int seed)
{
    srand(seed);
    //
    for(int i=0;i<n;i++)
    {
        for(int j=0;j<m;j++)
        {
            a[i][j] = rand() * (max-min)/RAND_MAX + min;
        }
    }
}

//
//复制；
Matrix Matrix::Copy(void)
{
    Matrix ret;
    ret.NewMatrix(n,m);
    for(int i=0;i<n;i++)
    {
        for(int j=0;j<m;j++)
            ret.a[i][j] = a[i][j];
        //
    }
    //
    return ret;
}

//从一个行顺序的数组输入；
void Matrix::Phase(double * rowOrderElement)
{
    for(int i=0;i<n;i++)
    {
        for(int j=0;j<m;j++)
        {
            a[i][j] = rowOrderElement[i * m + j];
        }
    }
}

//设置某个元素的值；
void Matrix::SetElement(int i,int j,double value)
{
    a[i][j] = value;
}

//
//从标准输入读矩阵；
void Matrix::ReadStd()
{
    for(int i=0;i<n;i++)
    {
        for(int j=0;j<m;j++)
        {
            cin >> (a[i][j]);
        }
        //
    }
}

//
//输出矩阵；
void Matrix::Print()
{
    //cout << endl;
    for(int i=0;i<n;i++)
    {
        for(int j=0;j<m;j++)
            cout << (a[i][j]) << " ";
        //
        cout << endl;
    }
}

//
//行化简，返回交换行的次数；
int Matrix::RowSimplify()
{
    int i,j,k,s,t;
    int swp=0;//返回值，表示行交换的次数；
    //
    for(i=0,j=0;i<n-1 && j<m;i++,j++)
    {
        //a[i][j]为要化简的列的第一行；
        //先把当前列最大的一行移上来，减少误差；
        for(k=i+1;k<n;k++)
        {
            if(Abs(a[i][j]) < Abs(a[k][j]))
            {
                double * tmp = a[k];
                a[k] = a[i];
                a[i] = tmp;
                //
                swp ++;
            }
        }
        //
        if(Equal(a[i][j],0))
        {
            //整列都为0；
            i--;//平移；
            continue;
        }
        //
        for(s=i+1;s<n;s++)//遍历行；
        {
            if(Equal(a[s][j],0))
                continue;//已经是0，本行不用处理；
            //
            double tmp = a[s][j] / a[i][j];
            //
            for(t=j;t<m;t++)//列遍历；
                a[s][t] -= a[i][t] * tmp;
            //
        }
    }
    return swp;
}

//
//求前n列的行列式，先要经过行化简，参数swp为行化简的返回值；
double Matrix::Det(int swp)
{
    if(m<n)//列数不够；
        throw;
    //
    double s=1;
    for(int i=0;i<n;i++)
        s *= a[i][i];
    //
    if(swp%2)
        return -s;
    return s;
}

//
//求矩阵的秩，要先行化简；
int Matrix::Rank()
{
    int r=n<m ? n : m;
    //
    //从下往上寻找第一个非零值；
    for(int i=n-1;i>=0;i--)
    {
        for(int j=0;j<m;j++)
        {
            if(!Equal(a[i][j],0))
                return r;
            //
        }
        r = r<i ? r : i;
    }
    //
    return r;
}

//
//化行最简型，先要经过行化简，返回(任意常量数+1)，例如1表示唯一解；
int Matrix::RowSimplifyMaxly(void)
{
    int row = m;//返回值，表示(解的任意常量数+1)；
    //
    int i,j,k,s,t;
    double tmp;
    //
    for(i=0,j=0;i<n&&j<m;i++,j++)
    {
        if(Equal(a[i][j],0))//平移，找出本行第一个非零；
        {
            i--;
            continue;
        }
        //
        row --;//少一个常量；
        //
        //化a[i][j]为1；
        tmp = 1 / a[i][j];
        for(k=j;k<m;k++)//前面的"0"就不处理了；
            a[i][k] *= tmp;
        //
        //化a[s][j]为0
        for(s=0;s<i;s++)//下面的0也不用处理；
        {
            if(Equal(a[s][j],0))
                continue;//已经为0；
            //
            tmp = a[s][j] / a[i][j];
            for(t=j;t<m;t++)
                a[s][t] -= a[i][t]*tmp;
            //
        }
    }
    //
    return row;
}

//
//解线性方程组，以增广矩阵形式输入，先要化行最简；
//C为任意常数，解出后放在sol；
void Matrix::Solve(double * C,double * sol)
{
    int i,j,k,c_i=0;
    //
    for(k=0;k<m-1;k++)//初始化解；
        sol[k] = 0;
    //
    int tab[MAX_SOL];//未知数与任意常数的对应表；
    for(k=0;k<MAX_SOL;k++)
        tab[k] = -1;
    //
    //填tab；
    for(i=0,j=0;i<n&&j<m-1;i++,j++)
    {
        if(Equal(a[i][j],0))//平移，找出本行第一个非零；
        {
            i--;
            tab[j] = c_i++;//当前列对应当前未知数；
            continue;
        }
    }
    for(;j<m-1;j++)//处理剩下的列；
        tab[j] = c_i++;
    //
    //计算解；
    for(i=0,j=0;i<n&&j<m;i++,j++)
    {
        if(Equal(a[i][j],0))//平移，找出本行第一个非零；
        {
            //处理x[j] = C[?]的情况；
            sol[j] = C[ tab[j] ];
            //
            i--;
            continue;
        }
        for(k=j+1;k<m-1;k++)
        {
            if(tab[k] >= 0)
                sol[j] -= C[ tab[k] ] * a[i][k];
        }
        sol[j] += a[i][m-1];
        //
    }
}

//
//检查是否正确解；
bool Matrix::IsSolution(double * sol)
{
    for(int i=0;i<n;i++)
    {
        double sum=0;
        for(int j=0;j<m-1;j++)
            sum += a[i][j] * sol[j];
        //
        if(!Equal(sum,a[i][m-1]))
            return false;
        //
    }
    //
    return true;
}

//
//测试方程组是否解正确；
void Matrix::TestSolve()
{
    int test_sum=0;
    //
    /*/输入；
    char * ssb = "  \
        2           \
        4 5         \
        1 1 1 2 19  \
        2 1 3 1 24  \
        2 2 1 2 24  \
        3 1 2 1 22  \
        4 5         \
        2 3 -1 -7 0 \
        3 1 2 -7 0  \
        4 1 -3 6 0  \
        1 -2 5 -5 0 \
        ";
    strstream sst(ssb,strlen(ssb));
    cin = sst;//*/
    //
    cin >> test_sum;
    for(int count=0;count<=test_sum;count++)
    {
        int n,m;
        cin >> n >> m;
        //
        Matrix mat,org;
        //
        mat.NewMatrix(n,m);
        if(count < test_sum)
            mat.ReadStd();
        else
            mat.Random(0,1000,10564);
        //
        org = mat.Copy();
        mat.RowSimplify();
        mat.RowSimplifyMaxly();
        //
        double C[] = {1,1,1,1,1,1,1};
        double solu[MAX_SOL];
        mat.Solve(C,solu);
        //
        if(org.IsSolution(solu))
            cout << count << ".SolveRight" << endl;
        else
            cout << count << ".SolveWrong" << endl;
        //
        mat.DeleteMatrix();
    }
    cout << endl;
}

//
//测试秩是否计算正确；
void Matrix::TestRank()
{
    int test_sum=0;
    //
    /*/输入
    char * ssb = "\
    3           \
    4 5         \
    2 -1 -1 1 2 \
    1 1 -2 1 4  \
    4 -6 2 -2 4 \
    3 6 -9 7 9  \
    3 3         \
    1 2 3       \
    2 3 -5      \
    4 7 1       \
    4 5         \
    3 2 0 5 0   \
    3 -2 3 6 -1 \
    2 0 1 5 -3  \
    1 6 -4 -1 4 \
    ";
    strstream sst(ssb,strlen(ssb));
    cin = sst;//*/
    //答案；
    double ans[] = {3,2,3};
    //
    cin >> test_sum;
    for(int count=0;count<test_sum;count++)
    {
        int n,m;
        cin >> n >> m;
        //
        Matrix mat;
        mat.NewMatrix(n,m);
        mat.ReadStd();
        mat.RowSimplify();
        //
        //mat.Print();
        if(Equal(mat.Rank(),ans[count]))
            cout << count << ".RankRight" << endl;
        else
            cout << count << ".RankWrong" << endl;
        //
        mat.DeleteMatrix();
    }
    cout << endl;
}

//
//测试行列式是否计算正确；
void Matrix::TestDet()
{
    int test_sum=0;
    //
    /*/输入；
    char * ssb = "  \
    4               \
    1 1             \
    5               \
    4 4             \
    1 1 1 1         \
    1 2 4 8         \
    1 3 9 27        \
    1 4 16 64       \
    3 3             \
    2 0 1           \
    1 -4 -1         \
    -1 8 3          \
    4 4             \
    4 1 2 4         \
    1 2 0 2         \
    10 5 2 0        \
    0 1 1 7         \
    ";
    strstream sst(ssb,strlen(ssb));
    cin = sst;//*/
    //答案；
    double ans[] = {5,12,-4,0};
    //
    cin >> test_sum;
    for(int count=0;count<test_sum;count++)
    {
        int n,m;
        cin >> n >> m;
        //
        Matrix mat;
        mat.NewMatrix(n,m);
        mat.ReadStd();
        int swp = mat.RowSimplify();
        //
        if(Equal(mat.Det(swp),ans[count]))
            cout << count << ".DetRight" << endl;
        else
            cout << count << ".DetWrong" << endl;
        //
        mat.DeleteMatrix();
    }
    cout << endl;
}

//~ int main(void)
//~ {
    //~ Matrix::TestRank();
    //~ Matrix::TestDet();
    //~ Matrix::TestSolve();
    //~ //
    //~ return 0;
//~ }
