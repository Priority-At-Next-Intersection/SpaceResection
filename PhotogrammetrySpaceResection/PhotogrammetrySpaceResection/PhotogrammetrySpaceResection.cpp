// PhotogrammetrySpaceResection.cpp: 定义控制台应用程序的入口点。
//
#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include <vector>
#include <string>
#include "Matrix.h"
#include <iostream>
//coordinate in image
struct imgPt
{
	double x;
	double y;
};
//coordinate in ground
struct gPt
{
	double X;
	double Y;
	double Z;
};

int main()
{
	//1. Read data files
	//1.1 Read controlPoints.txt
	std::vector<imgPt> imgPts;
	std::vector<gPt> gPts;
	std::string control_Pts_File_Name = "controlPoints.txt";
	FILE* fp_CP = fopen(control_Pts_File_Name.data(), "r");
	if (fp_CP==nullptr)
	{
		printf("ERROR: Cannot Open controlPoints.txt\n");
		return 1;
	}
	else
	{
		while (!feof(fp_CP))
		{
			double x, y, X, Y, Z;
			imgPt t_imgPt;
			gPt t_gPt;
			fscanf(fp_CP, "%lf,%lf,%lf,%lf,%lf,", &x, &y, &X, &Y, &Z);
			printf("read Points:%lf,%lf,%lf,%lf,%lf\n", x, y, X, Y, Z);
			t_imgPt.x = x;
			t_imgPt.y = y;
			t_gPt.X = X;
			t_gPt.Y = Y;
			t_gPt.Z = Z;
			imgPts.push_back(t_imgPt);
			gPts.push_back(t_gPt);
		}
	}


	//test
	Matrix A(5, 5);
	A.rand_init(1000);
	Matrix invOfA(5, 5);
	A.LUP_inverse(invOfA);
	A.show();

	Matrix B(3, 2);
	Matrix C(2, 3);
	B.rand_init(100);
	C.rand_init(100);
	Matrix result(3, 3);
	B.multiply(C, result);
	B.show();
	C.show();
	result.show();
    return 0;
}

