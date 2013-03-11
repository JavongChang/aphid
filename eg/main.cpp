// minimal linear system Ax=b example 
// numbers from http://people.fh-landshut.de/~maurer/femeth/node24.html
#include <iostream>
#include <Eigen/Dense>
#include <Eigen/LU>
#include <Eigen/Cholesky>
using namespace Eigen;

void testSimple()
{
    std::cout<<"LU\n";
    
    MatrixXf m(3,3);
    m(0,0) = 4;
    m(0,1) = 1;
    m(0,2) = 0;
    m(1,0) = 2;
    m(1,1) = 4;
    m(1,2) = 1;
    m(2,0) = 1;
    m(2,1) = 2;
    m(2,2) = 2;
    
    VectorXf b(3);
    b(0) = 6;
    b(1) = 13;
    b(2) = 11;
    std::cout << "m\n" << m << std::endl;
    std::cout << "b\n" << b << std::endl;
    
    
    LU<MatrixXf> lu(m);
    VectorXf x;
    lu.solve(b, &x);
    std::cout << "x\n" << x << std::endl;
    
}

void testLLT()
{
    std::cout<<"LLT\n";
    MatrixXf D = MatrixXf::Random(8,6);
    D = D * D.adjoint();
    MatrixXf m = MatrixXf::Random(8,6);
    D += m * m.adjoint();
    D(3, 0) = 0.f;
    D(3, 1) = 0.1f;
    D(3, 2) = 0.2f;
    D(3, 3) = 1.f;
    std::cout<<"D\n"<<D<<std::endl;
    
    VectorXf b = VectorXf::Random(8);
    std::cout<<"b\n"<<b<<std::endl;
    
    MatrixXf A = D.transpose() * D;
    
    VectorXf b1 = D.transpose() * b;
    
    LLT<MatrixXf> llt(A);
    
    VectorXf x;
    llt.solve(b1,&x);
    std::cout << "x\n" << x << std::endl;
    std::cout << "check Ax\n" << D * x << std::endl;
}

int main()
{
    testSimple();
    testLLT();
    return 0;
}

