#include "stdafx.h"
#include "Matrix.h"

using namespace std;

Matrix::Matrix(int row, int col)//������
{
	this->data = new double[row*col];
	this->row = row;
	this->col = col;
}
Matrix::~Matrix()
{
	delete[] data;
}
void Matrix::LUP_inverse(Matrix &invOfA)//LU�ֽ����棬�����¾����ָ��
{
	if (row != col)
	{
		printf("ERROR: col != row, fail to inverse\n");
		return ;
	}
	else {
		this->N = this->row;
		invOfA.data = LUP_solve_inverse(this->data);//��Ҫ�Ľ�����ǰ�ڴ�й©��
		return;
	}
}


//����˷�
double * Matrix::mul(double *A, double *B)
{
	double *C = new double[N*N]{};
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			for (int k = 0; k < N; k++)
			{
				C[i*N + j] += A[i*N + k] * B[k*N + j];
			}
		}
	}

	//������ֵС��10^-10,����Ϊ0���������Լ����ģ�
	for (int i = 0; i < N*N; i++)
	{
		if (abs(C[i]) < pow(10, -10))
		{
			C[i] = 0;
		}
	}

	return C;
}

//LUP�ֽ�
void Matrix::LUP_Descomposition(double *A, double *L, double *U, int *P)
{
	int row = 0;
	for (int i = 0; i < N; i++)
	{
		P[i] = i;
	}
	for (int i = 0; i < N - 1; i++)
	{
		double p = 0.0;
		for (int j = i; j < N; j++)
		{
			if (abs(A[j*N + i]) > p)
			{
				p = abs(A[j*N + i]);
				row = j;
			}
		}
		if (0 == p)
		{
			cout << "�������죬�޷�������" << endl;
			return;
		}

		//����P[i]��P[row]
		int tmp = P[i];
		P[i] = P[row];
		P[row] = tmp;

		double tmp2 = 0.0;
		for (int j = 0; j < N; j++)
		{
			//����A[i][j]�� A[row][j]
			tmp2 = A[i*N + j];
			A[i*N + j] = A[row*N + j];
			A[row*N + j] = tmp2;
		}

		//����ͬLU�ֽ�
		double u = A[i*N + i], l = 0.0;
		for (int j = i + 1; j < N; j++)
		{
			l = A[j*N + i] / u;
			A[j*N + i] = l;
			for (int k = i + 1; k < N; k++)
			{
				A[j*N + k] = A[j*N + k] - A[i*N + k] * l;
			}
		}

	}

	//����L��U
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j <= i; j++)
		{
			if (i != j)
			{
				L[i*N + j] = A[i*N + j];
			}
			else
			{
				L[i*N + j] = 1;
			}
		}
		for (int k = i; k < N; k++)
		{
			U[i*N + k] = A[i*N + k];
		}
	}

}

//LUP��ⷽ��
double * Matrix::LUP_Solve(double *L, double *U, int *P, double *b)
{
	double *x = new double[N]();
	double *y = new double[N]();

	//�����滻
	for (int i = 0; i < N; i++)
	{
		y[i] = b[P[i]];
		for (int j = 0; j < i; j++)
		{
			y[i] = y[i] - L[i*N + j] * y[j];
		}
	}
	//�����滻
	for (int i = N - 1; i >= 0; i--)
	{
		x[i] = y[i];
		for (int j = N - 1; j > i; j--)
		{
			x[i] = x[i] - U[i*N + j] * x[j];
		}
		x[i] /= U[i*N + i];
	}
	return x;
}

/*****************����ԭ��ת��BEGIN********************/

/* ��� */
int Matrix::getNext(int i, int m, int n)
{
	return (i%n)*m + i / n;
}

/* ǰ�� */
int Matrix::getPre(int i, int m, int n)
{
	return (i%m)*n + i / m;
}

/* �������±�iΪ���Ļ� */
void Matrix::movedata(double *mtx, int i, int m, int n)
{
	double temp = mtx[i]; // �ݴ�
	int cur = i;    // ��ǰ�±�
	int pre = getPre(cur, m, n);
	while (pre != i)
	{
		mtx[cur] = mtx[pre];
		cur = pre;
		pre = getPre(cur, m, n);
	}
	mtx[cur] = temp;
}

/* ת�ã���ѭ���������л� */
void Matrix::transpose(double *mtx, int m, int n)
{
	for (int i = 0; i < m*n; ++i)
	{
		int next = getNext(i, m, n);
		while (next > i) // �����ں��С��i˵���ظ�,�Ͳ�������ȥ�ˣ�ֻ�в��ظ�ʱ����whileѭ����
			next = getNext(next, m, n);
		if (next == i)  // ����ǰ��
			movedata(mtx, i, m, n);
	}
}
/*****************����ԭ��ת��END********************/

//LUP����(��ÿ��b����ĸ���x������װ)
double * Matrix::LUP_solve_inverse(double *A)
{
	//��������A�ĸ�����ע�ⲻ��ֱ����A���㣬��ΪLUP�ֽ��㷨�ѽ���ı�
	double *A_mirror = new double[N*N]();
	double *inv_A = new double[N*N]();//���յ�����󣨻���Ҫת�ã�
	double *inv_A_each = new double[N]();//������ĸ���
										 //double *B    =new double[N*N]();
	double *b = new double[N]();//b��ΪB����о������

	for (int i = 0; i < N; i++)
	{
		double *L = new double[N*N]();
		double *U = new double[N*N]();
		int *P = new int[N]();

		//���쵥λ���ÿһ��
		for (int i = 0; i < N; i++)
		{
			b[i] = 0;
		}
		b[i] = 1;

		//ÿ�ζ���Ҫ���½�A����һ��
		for (int i = 0; i < N*N; i++)
		{
			A_mirror[i] = A[i];
		}

		LUP_Descomposition(A_mirror, L, U, P);

		inv_A_each = LUP_Solve(L, U, P, b);
		memcpy(inv_A + i * N, inv_A_each, N * sizeof(double));//������ƴ������
	}
	transpose(inv_A, N, N);//�������ڸ���ÿ��b�����x���д洢�������ת��

	return inv_A;
}

