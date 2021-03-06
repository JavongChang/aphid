#ifndef LINEARMATH_H
#define LINEARMATH_H

#include <iostream>
#include <sstream>
#include <cmath>

#include "clapackTempl.h"

namespace lfr {

template<typename T>
class DenseVector {
   
    T * m_v;
    int m_numElements, m_capacity;
	bool m_isReferenced;
    
public:
    DenseVector(); 
	DenseVector(int n);             
	DenseVector(T * v, int n);
    virtual ~DenseVector();
    
    void create(int n);
	void resize(int n);
    int numElements() const;
    
    T& operator[](const int i);
    T operator[](const int i) const;
    
    T* v() const;
	T* raw();
    
    void setZero();
	void setOne();
	void setValue(const T x);
/// l2-norm ||x|| 
	T norm() const;
/// dot(x, x)
	T normSq() const;
	void scale(const T s);
    void normalize();
/// element index of minimum value
	int minInd(int range) const;
/// element index of max value
	int maxInd() const;
/// max element value
	T maxVal() const;
/// element index of max absolute value
	int maxAbsInd() const;
/// max absolute element value
	T maxAbsVal() const;
/// element index of min absolute value
	int minAbsInd(int range) const;
/// min absolute element value
	T minAbsVal(int range) const;
/// sum of element values
	T sumVal() const;
/// sum of absolute element values
	T sumAbsVal() const;
/// dot product
/// http://mathworld.wolfram.com/VectorMultiplication.html
	T dot(const DenseVector<T> & x) const;
	T dot(const T * x) const;
	
	void add(const DenseVector<T> & x, const T alpha = 1.0);
	void add(T * x, const T alpha = 1.0);
	void minus(const DenseVector<T> & x, const T alpha = 1.0);
	void copy(const DenseVector<T> & x);
	void copyData(const T * x);
	
	static void PrintVector(char* desc, int n, T* a)
	{
		std::cout<<"\n "<<desc<<"\n| ";
		int i;
		for(i=0; i< n; i++) {
			std::cout<<" "<<a[i];
		}
		std::cout<<" |\n";
	}
	
	friend std::ostream& operator<<(std::ostream &output, const DenseVector<T> & p) {
        output << p.str();
        return output;
    }
protected:
    
private:
	const std::string str() const;
    void clear();
};

template<typename T>
DenseVector<T>::DenseVector() : m_v(NULL), m_numElements(0), m_capacity(0), m_isReferenced(false) {}

template<typename T>
DenseVector<T>::DenseVector(int n) : m_numElements(n), m_capacity(n), m_isReferenced(false)
{
	m_v = new T[n];
}

template<typename T>
DenseVector<T>::DenseVector(T * v, int n) : m_v(v), m_numElements(n), m_capacity(0), m_isReferenced(true) {}

template<typename T>
DenseVector<T>::~DenseVector()
{ clear(); }

template<typename T>
void DenseVector<T>::create(int n)
{
    clear();
    m_v = new T[n];
    m_numElements = n;
	m_capacity = n;
}

template<typename T>
void DenseVector<T>::resize(int n)
{
	if(m_capacity >= n && !m_isReferenced) {
		m_numElements = n;
	}
	else {
		create(n);
	}
}

template<typename T>
int DenseVector<T>::numElements() const
{ return m_numElements; }

template<typename T>
T& DenseVector<T>::operator[](const int i)
{ return m_v[i]; }

template<typename T>
T DenseVector<T>::operator[](const int i) const
{ return m_v[i]; }

template<typename T>
T* DenseVector<T>::v() const
{ return m_v; }

template<typename T>
T* DenseVector<T>::raw()
{ return m_v; }

template<typename T>
void DenseVector<T>::setZero()
{ memset(m_v,0,m_numElements*sizeof(T)); }

template<typename T>
void DenseVector<T>::setOne()
{ setValue(1.0); }

template<typename T>
void DenseVector<T>::setValue(const T x)
{ for(int i = 0; i<m_numElements; i++) m_v[i] = x; }

template<typename T>
T DenseVector<T>::norm() const
{ return sqrt(normSq()); }

template<typename T>
T DenseVector<T>::normSq() const
{
	return clapack_dot<T>(m_numElements,m_v,1,m_v,1);
}

template<typename T>
void DenseVector<T>::scale(const T s)
{
	clapack_scal(m_numElements, s, m_v, 1);
}

template<typename T>
void DenseVector<T>::normalize()
{
	const T s = norm();
	if(s > 1e-5) scale(1.0 / s);
}

template<typename T>
int DenseVector<T>::minInd(int range) const
{
	int imin = 0;
	T vmin = m_v[0];
	for(int i=1; i<range; i++) {
		T cur = m_v[i];
		if(cur < vmin) {
			imin = i;
			vmin = cur;
		}
	}
	return imin;
}

template<typename T>
int DenseVector<T>::maxInd() const
{
	int imax = 0;
	T vmax = m_v[0];
	for(int i=1; i<m_numElements; i++) {
		T cur = m_v[i];
		if(cur > vmax) {
			imax = i;
			vmax = cur;
		}
	}
	return imax;
}

template<typename T>
T DenseVector<T>::maxVal() const
{ return m_v[maxInd()]; }

template<typename T>
int DenseVector<T>::maxAbsInd() const
{
	int imax = 0;
	T vmax = absoluteValue<T>(m_v[0]);
	for(int i=1; i<m_numElements; i++) {
		T cur = absoluteValue<T>(m_v[i]);
		if(cur > vmax) {
			imax = i;
			vmax = cur;
		}
	}
	return imax;
}

template<typename T>
T DenseVector<T>::maxAbsVal() const
{ return m_v[maxAbsInd()]; }

template<typename T>
int DenseVector<T>::minAbsInd(int range) const
{ 
	int imin = 0;
	T vmin = absoluteValue<T>(m_v[0]);
	for(int i=1; i<range; i++) {
		T cur = absoluteValue<T>(m_v[i]);
		if(cur < vmin) {
			imin = i;
			vmin = cur;
		}
	}
	return imin; 
}

template<typename T>
T DenseVector<T>::minAbsVal(int range) const
{ return m_v[minAbsInd()]; }

template<typename T>
T DenseVector<T>::sumVal() const
{
	T s = m_v[0];
	for(int i=1; i<m_numElements; i++) s += m_v[i];
	return s;
}

template<typename T>
T DenseVector<T>::sumAbsVal() const
{ 
	T s = absoluteValue<T>(m_v[0]);
	for(int i=1; i<m_numElements; i++) s += absoluteValue<T>(m_v[i]);
	return s;
}

template<typename T>
T DenseVector<T>::dot(const DenseVector<T> & x) const
{ return dot(x.v()); }

template<typename T>
T DenseVector<T>::dot(const T * x) const
{
	T s = 0.0;
	for(int i=0; i<m_numElements; i++) s += m_v[i] * x[i];
	return s;
}

template<typename T>
void DenseVector<T>::add(const DenseVector<T> & x, const T alpha)
{ add(x.v(), alpha); }

template<typename T>
void DenseVector<T>::add(T * x, const T alpha)
{ clapack_axpy<T>(m_numElements, alpha, x, 1, m_v, 1); }

template<typename T>
void DenseVector<T>::minus(const DenseVector<T> & x, const T alpha)
{ add(x.v(), -alpha); }

template<typename T>
void DenseVector<T>::copy(const DenseVector<T> & x)
{
	create(x.numElements());
	copyData(x.v());
}

template<typename T>
void DenseVector<T>::copyData(const T * x)
{ memcpy(m_v, x, m_numElements*sizeof(T)); }

template<typename T>
const std::string DenseVector<T>::str() const
{
	std::stringstream sst;
	sst<<" is "<<m_numElements<<" vector \n|";
	for (int i = 0; i<m_numElements; ++i) {
	  sst<<" "<<static_cast<double>(m_v[i]);
   }
   sst<<" |\n";
   return sst.str();
}

template<typename T>
void DenseVector<T>::clear()
{
    if(m_v) {
        if(!m_isReferenced) delete[] m_v;
    }
	m_v = NULL;
	m_numElements = 0;
	m_capacity = 0;
	m_isReferenced = false;
}

/// column-major dense matrix
/// n row is 1st dimension
/// n col is 2nd dimension
template<typename T>
class DenseMatrix {
	friend class DenseVector<T>;
    T * m_v;
    int m_numColumns;
    int m_numRows;
    int m_capacity;
	
public:
    DenseMatrix();
	DenseMatrix(int numRow, int numCol);
    virtual ~DenseMatrix();
    
    void create(int numRow, int numCol);
	void resize(int numRow, int numCol);
/// resize one dimension after created 
	void resizeNumRow(int numRow);
	void resizeNumCol(int numCol);
	int numColumns() const;
    int numRows() const;
	
/// i is column index, j is row index
    T& operator()(const int i, const int j);
    T operator()(const int i, const int j) const;
	T* column(const int i) const;
	T* raw();
	void getColumn(DenseVector<T> & x, const int i) const;
	
	void copy(const DenseMatrix<T> & x);
	void copyColumn(const int i, const T * x);
	void copyData(const T * x);
	
	void setZero();
	void scale(const T s);
	void scaleColumn(const int i, const T s);
	void add(const DenseMatrix<T> & x, const T alpha = 1.0);
	void minus(const DenseMatrix<T> & x, const T alpha = 1.0);
	
/// normalize each column
	void normalize();
/// AT * A
	void AtA(DenseMatrix<T>& dst) const;
/// aii += diag
	void addDiagonal(const T diag);
/// copy upper-right part to lower-left part
	void fillSymmetric();
	bool inverseSymmetric();
/// b = alpha A^T * x + beta b
	void transMult(DenseMatrix<T>& b, const DenseMatrix<T>& x, 
            const T alpha = 1.0, const T beta = 0.0) const;
/// b = alpha A^T * x^T + beta b
	void transMultTrans(DenseMatrix<T>& b, const DenseMatrix<T>& x, 
            const T alpha = 1.0, const T beta = 0.0) const;
/// b = alpha A * x^T + beta b
	void multTrans(DenseMatrix<T>& b, const DenseMatrix<T>& x, 
            const T alpha = 1.0, const T beta = 0.0) const;
/// b = alpha A * x + beta b
	void mult(DenseMatrix<T>& b, const DenseMatrix<T>& x, 
            const T alpha = 1.0, const T beta = 0.0) const;
/// b = alpha A * x + beta b
	void mult(DenseVector<T>& b, const DenseVector<T>& x, 
            const T alpha = 1.0, const T beta = 0.0) const;
/// b = alpha AT * x + beta b
	void multTrans(DenseVector<T>& b, const DenseVector<T>& x, 
            const T alpha = 1.0, const T beta = 0.0) const;
/// b = xT * A
	void lefthandMult(DenseVector<T>& b, const DenseVector<T>& x, 
            const T alpha = 1.0, const T beta = 0.0) const;
/// A = b * b
/// by relatively robust representations
/// A is symmetric and positive semi-definite matrix
/// A = Z⋅D⋅ZT
/// D = D'⋅D'
/// b = Z⋅D'⋅ZT
	bool sqrtRRR(DenseMatrix<T>& b) const;
	
/// b = (AT A)^-1AT
	bool pseudoInverse(DenseMatrix<T>& b) const;
	
/// A <- A + alpha * vec1 * vec2^t
	void rank1Update(const DenseVector<T> & vec1, const DenseVector<T> & vec2, const DenseVector<int> & ind2,
						const int n2, const T alpha=1.0);
/// A <- A + alpha * vec * vec^t
	void rank1Update(const DenseVector<T> & vec1, const DenseVector<int> & ind,
						const int n, const T alpha=1.0);
	
	friend std::ostream& operator<<(std::ostream &output, const DenseMatrix<T> & p) {
        output << p.str();
        return output;
    }
	
	static void PrintMatrix(char* desc, int m, int n, T* a)
	{
		int i, j;
		std::cout<<"\n "<<desc;
		for(i=0; i< m; i++) {
			std::cout<<"\n| ";
			for(j=0; j< n; j++) {
				std::cout<<" "<<a[j*m + i];
			}
			std::cout<<" |";
		}
		std::cout<<"\n";
	}
	
	void printColumn(const int i) const;

protected:
    
private:
	const std::string str() const;
    void clear();
    
};

template<typename T>
DenseMatrix<T>::DenseMatrix() : m_v(NULL), m_numColumns(0), m_numRows(0), m_capacity(0) {}

template<typename T>
DenseMatrix<T>::DenseMatrix(int numRow, int numCol) : m_numColumns(numCol), m_numRows(numRow)
{ 
	m_capacity = numCol*numRow;
    m_v = new T[m_capacity];
}

template<typename T>
DenseMatrix<T>::~DenseMatrix() 
{ clear(); }

template<typename T>
void DenseMatrix<T>::create(int numRow, int numCol)
{
    clear();
    m_numColumns = numCol;
    m_numRows = numRow;
	m_capacity = numCol*numRow;
    m_v = new T[m_capacity];
}

template<typename T>
void DenseMatrix<T>::resize(int numRow, int numCol)
{
	if(m_capacity >= numRow*numCol) {
		m_numColumns = numCol;
		m_numRows = numRow;
	}
	else {
		create(numRow, numCol);
	}
}

template<typename T>
void DenseMatrix<T>::resizeNumRow(int numRow)
{ resize(numRow, m_numColumns); }

template<typename T>
void DenseMatrix<T>::resizeNumCol(int numCol)
{ resize(m_numRows, numCol); }

template<typename T>
int DenseMatrix<T>::numColumns() const
{ return m_numColumns; }

template<typename T>
int DenseMatrix<T>::numRows() const
{ return m_numRows; }

template <typename T> 
T& DenseMatrix<T>::operator()(const int i, const int j) 
{ return m_v[i*m_numRows+j]; }

template <typename T> 
T DenseMatrix<T>::operator()(const int i, const int j) const 
{ return m_v[i*m_numRows+j]; }

template <typename T>
T* DenseMatrix<T>::column(const int i) const
{ return &m_v[i*m_numRows]; }

template <typename T>
T* DenseMatrix<T>::raw()
{ return m_v; }

template <typename T>
void DenseMatrix<T>::getColumn(DenseVector<T> & x, const int i) const
{ memcpy(x.raw(), column(i), m_numRows*sizeof(T)); }

template <typename T>
void DenseMatrix<T>::copy(const DenseMatrix<T> & x)
{ memcpy(m_v, x.column(0), m_numRows*m_numColumns*sizeof(T)); }

template <typename T>
void DenseMatrix<T>::copyColumn(const int i, const T * x)
{ memcpy(&m_v[i*m_numRows], x, m_numRows*sizeof(T)); }

template <typename T>
void DenseMatrix<T>::copyData(const T * x)
{ memcpy(m_v, x, m_numRows * m_numColumns * sizeof(T)); }

template <typename T> 
void DenseMatrix<T>::setZero()
{
	for(int i = 0;i<m_numColumns;i++) {
		DenseVector<T> d(&m_v[i*m_numRows], m_numRows);
		d.setZero();
	}
}

template <typename T> 
void DenseMatrix<T>::scale(const T s)
{
	for(int i=0;i<m_numColumns;i++) scaleColumn(i, s);
}

template <typename T> 
void DenseMatrix<T>::scaleColumn(const int i, const T s)
{
	DenseVector<T> d(&m_v[i*m_numRows], m_numRows);
	d.scale(s);
}

template<typename T>
void DenseMatrix<T>::add(const DenseMatrix<T> & x, const T alpha)
{ clapack_axpy<T>(m_numRows*m_numColumns, alpha, x.column(0), 1, m_v, 1); }

template<typename T>
void DenseMatrix<T>::minus(const DenseMatrix<T> & x, const T alpha)
{ clapack_axpy<T>(m_numRows*m_numColumns, -alpha, x.column(0), 1, m_v, 1); }

template <typename T> 
void DenseMatrix<T>::normalize()
{
	int i = 0;
	for(;i<m_numColumns;i++) {
		DenseVector<T> d(&m_v[i*m_numRows], m_numRows);
		d.normalize();
	}
}

template <typename T>
void DenseMatrix<T>::AtA(DenseMatrix<T>& dst) const 
{
/// syrk performs a rank-n update of an n-by-n symmetric matrix c, that is:
/// c := alpha*a'*a + beta*c
/// a is k-by-n matrix
/// c is n-by-n matrix
/// alpha = 1, beta = 0 
	dst.resize(m_numColumns, m_numColumns);
	clapack_syrk<T>("U", "T", m_numColumns, m_numRows, 
										T(1.0), m_v, m_numRows, 
										T(0.0), dst.raw(), m_numColumns);
    dst.fillSymmetric();
}

template <typename T>
void DenseMatrix<T>::transMult(DenseMatrix<T>& b, const DenseMatrix<T>& x, 
            const T alpha, const T beta) const
{
	clapack_gemm<T>("T", "N", m_numColumns, m_numRows, m_numRows, 
							alpha, m_v, m_numRows, 
							x.column(0), x.numRows(), beta, b.raw(), m_numColumns);
}

template <typename T>
void DenseMatrix<T>::transMultTrans(DenseMatrix<T>& b, const DenseMatrix<T>& x, 
            const T alpha, const T beta) const
{
	clapack_gemm<T>("T", "T", m_numColumns, m_numRows, m_numRows, 
							alpha, m_v, m_numRows, 
							x.column(0), x.numRows(), beta, b.raw(), m_numColumns);
}

/// b = alpha A * x^T + beta b
/// M A.m
/// N x^T.n
/// K A.n
template <typename T>
void DenseMatrix<T>::multTrans(DenseMatrix<T>& b, const DenseMatrix<T>& x, 
            const T alpha, const T beta) const
{
	clapack_gemm<T>("N", "T", m_numRows, 
							x.numRows(), 
							m_numColumns, 
							alpha, m_v, m_numRows, 
							x.column(0), x.numRows(), beta, b.raw(), m_numRows);
}

/// reference http://www.math.utah.edu/software/lapack/lapack-blas/dgemm.html
/// b = alpha A * x + beta b
/// M A.m
/// N x.n
/// K A.n
template <typename T>
void DenseMatrix<T>::mult(DenseMatrix<T>& b, const DenseMatrix<T>& x, 
            const T alpha, const T beta) const
{
	clapack_gemm<T>("N", "N", m_numRows, 
							x.numColumns(), 
							m_numColumns, 
							alpha, m_v, m_numRows, 
							x.column(0), x.numRows(), beta, b.raw(), m_numRows);
}

template <typename T>
void DenseMatrix<T>::mult(DenseVector<T>& b, const DenseVector<T>& x, 
            const T alpha, const T beta) const
{
	clapack_gemv<T>("N", m_numRows, m_numColumns, 
							alpha, m_v, m_numRows, 
							x.v(), 1, 
							beta, b.v(), 1);
}

template <typename T>
void DenseMatrix<T>::multTrans(DenseVector<T>& b, const DenseVector<T>& x, 
            const T alpha, const T beta) const
{
	clapack_gemv<T>("T", m_numRows, m_numColumns, 
							alpha, m_v, m_numRows, 
							x.v(), 1, 
							beta, b.v(), 1);
}

/// http://www.math.utah.edu/software/lapack/lapack-blas/dgemm.html
template <typename T>
void DenseMatrix<T>::lefthandMult(DenseVector<T>& b, const DenseVector<T>& x, 
            const T alpha, const T beta) const
{
/// xT is 1-by-m vector(matrix)
/// b is 1-by-n vector(matrix)
	clapack_gemm<T>("T", "N", 1, m_numColumns, m_numColumns, 
							alpha, x.v(), x.numElements(), 
							m_v, m_numRows, beta, b.raw(), 1);
}

template <typename T>
void DenseMatrix<T>::fillSymmetric() 
{
	for (int i = 0; i<m_numColumns; ++i) {
      for (int j =0; j<i; ++j) {
         m_v[j*m_numRows+i]=m_v[i*m_numRows+j];
      }
   }
}

template<typename T>
bool DenseMatrix<T>::inverseSymmetric()
{
	T * work;
	T queryWork;
	work = &queryWork;
	integer * ipiv = new integer[m_numRows];
	int i;
	for(i=0;i<m_numRows;i++) ipiv[i] = i;
	integer info;
	integer lwork = -1;
	clapack_sytrf<T>("U",m_numRows,m_v,m_numRows, ipiv, work, &lwork, &info);
	lwork = work[0];
	work = new T[lwork];
	clapack_sytrf<T>("U",m_numRows,m_v,m_numRows, ipiv, work, &lwork, &info);
	clapack_sytri<T>("U",m_numRows,m_v,m_numRows, ipiv, work, &info);
	fillSymmetric();
	delete[] work;
	delete[] ipiv;
	return info==0;
}

template <typename T>  
void DenseMatrix<T>::addDiagonal(const T diag) 
{ 
	const int n = min(m_numRows, m_numColumns);
	for(int i = 0; i<n; ++i) 
		m_v[i*m_numRows+i] += diag; 
};

template <typename T>  
void DenseMatrix<T>::rank1Update(const DenseVector<T> & vec1, const DenseVector<T> & vec2, const DenseVector<int> & ind2,
						const int n2, const T alpha)
{
	int * r = ind2.v();
	T* v = vec2.v();
	for (int i = 0; i<n2; ++i) {
		DenseVector<T> Xi(column(r[i]), numRows());
		Xi.add(vec1,v[i]*alpha);
	}
}

template <typename T>  
void DenseMatrix<T>::rank1Update(const DenseVector<T> & vec1, const DenseVector<int> & ind,
	const int n, const T alpha) 
{
	int * r = ind.v();
	T* v = vec1.v();
	int i, j;
	if (alpha == 1.0) {
	  for (i = 0; i<n; ++i) {
		 for (j = 0; j<n; ++j) {
			column(r[i])[r[j]] += v[j]*v[i];
		 }
	  }
	} else {
	  for (i = 0; i<n; ++i) {
		 for (j = 0; j<n; ++j) {
			column(r[i])[r[j]] += alpha*v[j]*v[i];
		 }
	  }
	}
};

/// http://scc.qibebt.cas.cn/docs/library/Intel%20MKL/2011/mkl_manual/lse/functn_syevr.htm
/// all eigenvalues and eigenvectors
template <typename T> 
bool DenseMatrix<T>::sqrtRRR(DenseMatrix<T>& b) const
{
	T * W = new T[m_numRows];
	T * Z = new T[m_numRows*m_numRows];
	integer * ISuppz = new integer[2*m_numRows];
	
	T * work;
	integer * iwork;
	T abstol = -1.0;
	T vl, vu;
	int il = 1;
	int iu = m_numRows;
	integer m;
	integer info;
	integer lwork = -1;
	integer liwork = -1;
	T queryWork; work = &queryWork;
	integer queryIwork; iwork = &queryIwork;
	
	clapack_syevr<T>("V", "A", "U", m_numRows, m_v, m_numRows, 
		&vl, &vu, il, iu, abstol, &m,
         W, Z, m_numRows, ISuppz, 
		 work, &lwork, iwork, &liwork, &info);
		 
	lwork = queryWork;
	liwork = queryIwork;
	
	work = new T[lwork];
	iwork = new integer[liwork];
	
	clapack_syevr<T>("V", "A", "U", m_numRows, m_v, m_numRows, 
		&vl, &vu, il, iu, abstol, &m,
         W, Z, m_numRows, ISuppz, 
		 work, &lwork, iwork, &liwork, &info);
		 
/// B = Z * D		 
	T * B = new T[m_numRows*m_numRows];
	int i, j;
    for(i=0; i< m_numRows; i++) {
		double  lambda=sqrt(W[i]);
        for(j=0; j< m_numRows; j++) {
            B[i*m_numRows + j] = Z[i*m_numRows + j] * lambda;
        }
    }

/// b = B * ZT	
	clapack_gemm<double>("N", "T", m_numRows, m_numRows, m_numRows,
						1.0, B, m_numRows, Z, m_numRows, 0.0, b.raw(), m_numRows);
						
	delete[] work;
	delete[] iwork;
	delete[] B;
	delete[] W;
	delete[] Z;
	delete[] ISuppz;
	return info == 0;
}

template<typename T>
const std::string DenseMatrix<T>::str() const
{
	std::stringstream sst;
	sst<<"\n "<<m_numRows<<"-by-"<<m_numColumns<<" matrix ";
	for (int i = 0; i<m_numRows; ++i) {
      sst<<"\n|";
	  for (int j = 0; j<m_numColumns; ++j) {
         sst<<" "<<m_v[j*m_numRows+i];
      }
      sst<<" |";
   }
   sst<<"\n";
   return sst.str();
}

template<typename T>
void DenseMatrix<T>::clear() 
{ 
    if(m_v) {
        delete[] m_v;
        m_v = NULL;
    }
    m_numColumns = 0;
    m_numRows = 0;
	m_capacity = 0;
}

/// column-major sparse matrix in csr

template<typename T>
class SparseMatrix {

    T * m_v;
    int * m_rowIndices;
    int * m_columnBegins;
    int m_numColumns;
    int m_numRows;
    int m_numMaxNonZero;
    
public:
    SparseMatrix();
    virtual ~SparseMatrix();
    
    void create(int numRow, int numCol, int maxNz);
    int numColumns() const;
    int numRows() const;
    int maxNumNonZero() const;
    
protected:

private:
    void clear();
};

template<typename T>
SparseMatrix<T>::SparseMatrix() : m_v(NULL), m_rowIndices(NULL), m_columnBegins(NULL), m_numMaxNonZero(0) {}

template<typename T>
SparseMatrix<T>::~SparseMatrix()
{ clear(); }

template<typename T>
void SparseMatrix<T>::create(int numRow, int numCol, int maxNz)
{
    clear();
    m_numColumns = numCol;
    m_numRows = numRow;
    m_numMaxNonZero = maxNz;
    m_v = new T[maxNz];
    m_rowIndices = new int[maxNz];
    m_columnBegins = new int[numCol+1];
}

template<typename T>
int SparseMatrix<T>::numColumns() const
{ return m_numColumns; }

template<typename T>
int SparseMatrix<T>::numRows() const
{ return m_numRows; }

template<typename T>
int SparseMatrix<T>::maxNumNonZero() const
{ return m_numMaxNonZero; }

template<typename T>
void SparseMatrix<T>::clear() 
{ 
    if(m_v) {
        delete[] m_v;
        m_v = NULL;
    }
    if(m_rowIndices) {
        delete[] m_rowIndices;
        m_rowIndices = NULL;
    }
    if(m_columnBegins) {
        delete[] m_columnBegins;
        m_columnBegins = NULL;
    }
    m_numColumns = 0;
    m_numRows = 0;
    m_numMaxNonZero = 0;
}

template<typename T>
class SvdSolver {

	lfr::DenseVector<T> m_s;
	lfr::DenseMatrix<T> m_u; 
	lfr::DenseMatrix<T> m_v;
	
	T * m_work;
	int m_l;
public:
	SvdSolver();
	virtual ~SvdSolver();
	
	bool compute(const DenseMatrix<T> & M);
	
	const DenseMatrix<T> & U() const;
	const DenseVector<T> & S() const;
	const DenseMatrix<T> & Vt() const;
	
	DenseVector<T> * SR();
	
protected:

private:

};

template<typename T>
SvdSolver<T>::SvdSolver() 
{ 
	m_l = 0;
	m_work = NULL; 
}

template<typename T>
SvdSolver<T>::~SvdSolver() 
{ if(m_work) free(m_work); }

/// A mxn = U mxm S mxn V^T nxn
/// M is changed
/// V is V^T actually
template<typename T>
bool SvdSolver<T>::compute(const DenseMatrix<T> & M)
{
	const int m = M.numRows();
	const int n = M.numColumns();
	
	m_s.resize(m );
	m_u.resize(m, m );
	m_v.resize(n, n );
	
	T wkopt;

/// Query and allocate the optimal workspace
	integer lwork = -1, info;
	clapack_gesvd<T>( "All", "All", m, n, M.column(0), m, m_s.raw(), m_u.column(0), m, m_v.column(0), n, &wkopt, &lwork, &info );
	lwork = (int)wkopt;
	// std::cout<<"\n work l "<<lwork;
	
	if(m_l < lwork) {
		m_l = lwork;
		if(m_work) free(m_work);
		m_work = (T*)malloc( lwork*sizeof(T) );
	}
/// Compute SVD 
	clapack_gesvd<T>( "All", "All", m, n, M.column(0), m, m_s.raw(), m_u.column(0), m, m_v.column(0), n, m_work, &lwork, &info );
/// Check for convergence
	if( info > 0 ) {
			std::cout<< "The algorithm computing SVD failed to converge.\n";
			return false;
	}
	
	return true;
}

template<typename T>
const DenseMatrix<T> & SvdSolver<T>::U() const
{ return m_u; }

template<typename T>
const DenseVector<T> & SvdSolver<T>::S() const
{ return m_s; }

template<typename T>
const DenseMatrix<T> & SvdSolver<T>::Vt() const
{ return m_v; }	

template<typename T>
DenseVector<T> * SvdSolver<T>::SR()
{ return &m_s; }
	
} /// end of namespace lfr

#endif        //  #ifndef LINEARMATH_H

