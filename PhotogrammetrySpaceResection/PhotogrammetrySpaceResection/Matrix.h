#pragma once
#include <iostream>
#include <cmath>
#include <ctime>

class Matrix
{
public:
	double* data;
	int row;
	int col;
private:
	int N;
public:
	Matrix(int row, int col);//������
	~Matrix();
	void LUP_inverse(Matrix &invOfA);//LU�ֽ����棬�����¾����ָ��
	void multiply(Matrix& multiplierB, Matrix& result);//This*B=result
	void trans();//ת��
	void show();
	void rand_init(int scale);
private:
	//����˷�
	double * mul(double *A, double *B);

	//LUP�ֽ�
	void LUP_Descomposition(double *A, double *L, double *U, int *P);

	//LUP��ⷽ��
	double * LUP_Solve(double *L, double *U, int *P, double *b);

	/*****************����ԭ��ת��BEGIN********************/

	/* ��� */
	int getNext(int i, int m, int n);

	/* ǰ�� */
	int getPre(int i, int m, int n);

	/* �������±�iΪ���Ļ� */
	void movedata(double *mtx, int i, int m, int n);
	/* ת�ã���ѭ���������л� */
	void transpose(double *mtx, int m, int n);
	/*****************����ԭ��ת��END********************/

	//LUP����(��ÿ��b����ĸ���x������װ)
	double * LUP_solve_inverse(double *A);
};

