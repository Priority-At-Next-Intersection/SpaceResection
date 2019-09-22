#include "pch.h"
#include "RESECTION.h"




//���캯���������������������
RESECTION::RESECTION(string control_file_path0, string image_file_path0, string image0)
{
	cout << "******** Space Resection ********" << endl;
	cout << "Step 1: Read object space coordinates and pixel coordinates of the control point." << endl;
	readControlPts(control_file_path0);
	readImagePts(image_file_path0);
	control_file_path = control_file_path0;
	image_file_path = image_file_path0;
	image = image0;
	img = imread(image);
	namedWindow("iteration", cv::WINDOW_NORMAL);
	imshow("iteration", img);

	//��δ֪������ֵ(�����д��k2�ͺ���)
	cout << "Step 2: Assign initial values to 13 unknowns since we use Gauss-Newton Method." << endl;

	//f = 6157.84; //32mmΪ�������,5.1966e-3mmΪ�������ش�С��6157.84Ϊ������Ϊ����������ֵ
	f = 3940;
	k1 = 0;
	k2 = 0;
	p1 = 0;
	p2 = 0;

	//����������Ҫ�õ��ľ������ռ�
	elementNum = 13;
	V = Mat(equationNum, 1, CV_32FC1);
	A = Mat(equationNum, elementNum, CV_32FC1);
	X = Mat(equationNum, 1, CV_32FC1);
	L = Mat(equationNum, 1, CV_32FC1);
	R = Mat(3, 3, CV_32FC1);

	itrNum = 0;
}
//�����������ͷŷ�����ڴ�
RESECTION::~RESECTION()
{
}
void RESECTION::Resection()
{
	cout << "Step 3: Initialize the error equation and then start using the Gauss-Newton method to calculate the value of the unknowns iteratively" << endl;
	//����UpdateRAL
	updateRAL();
	//����X����
	updateX();
	itrNum++;
	//Ȼ����while(��û����) ��UpdateRAL
	bool notConverge = (abs(X.at<float>(3, 0)) > 1e-5 || abs(X.at<float>(4, 0)) > 1e-5 || abs(X.at<float>(5, 0)) > 1e-5);
	while (notConverge)
	{
		updateRAL();
		updateX();
		notConverge = (abs(X.at<float>(3, 0)) > 1e-5 || abs(X.at<float>(4, 0)) > 1e-5 || abs(X.at<float>(5, 0)) > 1e-5);
		itrNum++;
	}
	//���������writeResults
	cout << "Step 4: Write results" << endl;
	cout << "******** The End ********" << endl;
	writeResults();
}
int RESECTION::readControlPts(string control_file_path)//���ܣ���ȡ���Ƶ������ļ�
{
	ifstream ifs;
	ifs.open(control_file_path);
	if (!ifs)
	{
		printf("Fail to read control points\n");
		return 0;
	}
	int pts_num = 0;
	//�ȶ�ȡ�ܵ���
	ifs >> pts_num;
	printf("  Num of Control Pts��%d\n", pts_num);
	controlPts = Mat(pts_num, 4, CV_32FC1);//32λ���㣬��ͨ����ʵ���Ͼ��Ǹ������
	float ID, X, Y, Z;
	int cnt = 0;
	while (ifs >> ID >> X >> Y >> Z)
	{
		if (disp)//������Ϣ
		{
			cout << ID << " " << X << " " << Y << " " << Z << endl;
		}
		//����һ����
		controlPts.at<float>(cnt, 0) = ID;
		controlPts.at<float>(cnt, 1) = X;
		controlPts.at<float>(cnt, 2) = Y;
		controlPts.at<float>(cnt, 3) = Z;
		cnt++;
	}
	ifs.close();
	return 1;
}

int RESECTION::readImagePts(string image_file_path)//���ܣ���ȡ��������ļ�
{
	ifstream ifs;
	ifs.open(image_file_path);
	if (!ifs)
	{
		printf("Fail to read image points\n");
		return 0;
	}
	int pts_num = 0;
	//������ɱ䣬����vector��ʱ��ȡ
	float ID, x, y;
	cout << "----------\nTip: This program is suitable for data obtained from close-range photogrammetry.\n If you need to use data acquired by aerial photogrammetry, try swapping the XYZ coordinates\n so that they match the close-up photogrammetry.\n----------" << endl;
	while (ifs >> ID >> x >> y)
	{
		//�ڱ���ʵ���д���δ֪�㣬�ڿռ�󷽽������ò��ϣ���Ҫ������
		//��Щδ֪�����Ŷ��� ��λ���������Ƶ����Ŷ��� ��λ�������Բ��������е���λ����
		//ע�⣺�����û����Щ��λ����ŵ����ݣ������ע�͵����if��䡣
		//There are unknown points in this experiment, which are useless in the resection of space, and therefore need to be skipped.
		//The numbers of these unknown points are two digits, and the numbers of the control points are all three digits, so point numbers which are lower than 100 are skipped.
		//Note: If it is normal data without useless two digits points, be sure to comment out the if statement.
		if (ID < 100)
		{
			continue;//����δ֪��
		}
		ImagePt temp;
		temp.ID = ID;
		temp.x = x;
		temp.y = y;
		//�ڿ��Ƶ�������֪�����ID��Ӧ��[X,Y,Z]
		for (int i = 0; i < controlPts.rows; i++)
		{
			if (controlPts.at<float>(i, 0) == ID)
			{
				//������Ҫ�任һ������ϵ----������Ƶ��Ǻ�����Ӱ�����µģ���ֱ�Ӱ�˳���ȡ123λ����
				//X = Yo��Y = Zo��Z = -Xo
				temp.X = controlPts.at<float>(i, 2);
				temp.Y = controlPts.at<float>(i, 3);
				temp.Z = -controlPts.at<float>(i, 1);
				break;//�Ϳ����˳���
			}
		}
		if (disp)//������Ϣ
		{
			cout << ID << " " << x << " " << y << " " << temp.X << " " << temp.Y << " " << temp.Z << endl;
		}
		imagePts.push_back(temp);
		pts_num++;
	}
	imagePtsNum = pts_num;
	equationNum = 2 * pts_num;
	ifs.close();
	return 1;
}

void RESECTION::updateRAL()//���¼������ R\A\L
{
	//�ȼ���R��R=R1 x R2 x R3;
	R.at<float>(0, 0) = cos(Phi)*cos(Kappa) - sin(Phi)*sin(Omega)*sin(Kappa);
	R.at<float>(0, 1) = cos(Omega)*sin(Kappa);
	R.at<float>(0, 2) = sin(Phi)*cos(Kappa) + cos(Phi)*sin(Omega)*sin(Kappa);
	R.at<float>(1, 0) = -cos(Phi)*sin(Kappa) - sin(Phi)*sin(Omega)*cos(Kappa);
	R.at<float>(1, 1) = cos(Omega)*cos(Kappa);
	R.at<float>(1, 2) = -sin(Phi)*sin(Kappa) + cos(Phi)*sin(Omega)*cos(Kappa);
	R.at<float>(2, 0) = -sin(Phi)*cos(Omega);
	R.at<float>(2, 1) = -sin(Omega);
	R.at<float>(2, 2) = cos(Phi)*cos(Omega);
	//�������������
	Mat imgt = img.clone();
	for (int i = 0; i < imagePtsNum; i++)
	{
		// ����ֵ[�±�i�����i����][һ���������������]
		//1.��ȡֵ
		double x = imagePts.at(i).x;//�۲�ֵ
		double y = imagePts.at(i).y;
		double Xi = imagePts.at(i).X;
		double Yi = imagePts.at(i).Y;
		double Zi = imagePts.at(i).Z;
		//2.�й��߷���
		double XR = R.at<float>(0, 0)*(Xi - Xs) + R.at<float>(0, 1)*(Yi - Ys) + R.at<float>(0, 2)*(Zi - Zs);
		double YR = R.at<float>(1, 0)*(Xi - Xs) + R.at<float>(1, 1)*(Yi - Ys) + R.at<float>(1, 2)*(Zi - Zs);
		double ZR = R.at<float>(2, 0)*(Xi - Xs) + R.at<float>(2, 1)*(Yi - Ys) + R.at<float>(2, 2)*(Zi - Zs);
		double xi = -f * XR / ZR + x0;//����ֵ
		double yi = -f * YR / ZR + y0;
		//����������ֵ����
		double r = sqrt((x - x0)*(x - x0) + (y - y0)*(y - y0));
		//����ֵ�ļ��㷽���ǣ�
			// x = (xi-x0)
			//dx = (x)[k1*r^2 + k2*r^4] + [2*p1*x*y + p2*(r^2+2*x^2)]
			//dy = (y)[k1*r^2 + k2*r^4] + [p1*(r^2+2*y^2) + 2*p2*x*y]
		double dx = (x - x0)*(k1 * powf(r, 2) + k2 * powf(r, 4)) + 2 * p1*(x - x0)*(y - y0) + p2 * (powf(r, 2) + 2 * powf((x - x0), 2));
		double dy = (y - y0)*(k1 * powf(r, 2) + k2 * powf(r, 4)) + p1 * (powf(r, 2) + 2 * powf((y - y0), 2)) + 2 * p2*(x - x0)*(y - y0);
		xi = (xi - dx);//������������ֵ
		yi = (yi - dy);
		//3.1.��һ�����̣���x) 
		L.at<float>(2 * i, 0) = x - xi;//�۲�ֵ-������������ֵ
		//(x-x0)[�۲�ֵ] = (xi-dx)[����ֵ] --> (xi - [�۲�ֵ]*����ϵ��)
		//����A����
		A.at<float>(2 * i, 0) = (R.at<float>(0, 0)*f + R.at<float>(2, 0)*(x - x0)) / ZR;//A���������ǹ۲�ֵ
		A.at<float>(2 * i, 1) = (R.at<float>(0, 1)*f + R.at<float>(2, 1)*(x - x0)) / ZR;
		A.at<float>(2 * i, 2) = (R.at<float>(0, 2)*f + R.at<float>(2, 2)*(x - x0)) / ZR;
		A.at<float>(2 * i, 3) = (y - y0)*sin(Omega) - (((x - x0) / f)*((x - x0)*cos(Kappa) - (y - y0)*sin(Kappa)) + f * cos(Kappa))*cos(Omega);
		A.at<float>(2 * i, 4) = -1 * f*sin(Kappa) - ((x - x0) / f)*((x - x0)*sin(Kappa) + (y - y0)*cos(Kappa));
		A.at<float>(2 * i, 5) = y - y0;
		A.at<float>(2 * i, 6) = (x - x0) / f;
		A.at<float>(2 * i, 7) = 1;
		A.at<float>(2 * i, 8) = 0;
		A.at<float>(2 * i, 9) = -1 * (x - x0) * powf(r, 2);
		A.at<float>(2 * i, 10) = -1 * (x - x0) * powf(r, 4);
		A.at<float>(2 * i, 11) = -1 * 2 * (x - x0)*(y - y0);
		A.at<float>(2 * i, 12) = -1 * (powf(r, 2) + 2 * powf((x - x0), 2));

		//3.2.�ڶ������̣���y��
		L.at<float>(2 * i + 1, 0) = y - yi;
		//����A����
		A.at<float>(2 * i + 1, 0) = (R.at<float>(1, 0)*f + R.at<float>(2, 0)*(y - y0)) / ZR;
		A.at<float>(2 * i + 1, 1) = (R.at<float>(1, 1)*f + R.at<float>(2, 1)*(y - y0)) / ZR;
		A.at<float>(2 * i + 1, 2) = (R.at<float>(1, 2)*f + R.at<float>(2, 2)*(y - y0)) / ZR;
		A.at<float>(2 * i + 1, 3) = -1 * (x - x0)*sin(Omega) - (((y - y0) / f)*((x - x0)*cos(Kappa) - (y - y0)*sin(Kappa)) - f * sin(Kappa))*cos(Omega);
		A.at<float>(2 * i + 1, 4) = -1 * f*cos(Kappa) - ((y - y0) / f)*((x - x0)*sin(Kappa) + (y - y0)*cos(Kappa));
		A.at<float>(2 * i + 1, 5) = -1 * (x - x0);
		A.at<float>(2 * i + 1, 6) = (y - y0) / f;
		A.at<float>(2 * i + 1, 7) = 0;
		A.at<float>(2 * i + 1, 8) = 1;
		A.at<float>(2 * i + 1, 9) = -1 * (y - y0) * powf(r, 2);
		A.at<float>(2 * i + 1, 10) = -1 * (y - y0) * powf(r, 4);
		A.at<float>(2 * i + 1, 11) = -1 * (powf(r, 2) + 2 * powf((y - y0), 2));
		A.at<float>(2 * i + 1, 12) = -1 * 2 * (x - x0)*(y - y0);
		//��ʾ���ε������
		line(imgt, Point2i(2136 + (xi)-15, 1424 - (yi)), Point2i(2136 + (xi)+15, 1424 - (yi)), Scalar(0, 0, 255), 2, LINE_AA, 0);
		line(imgt, Point2i(2136 + (xi), 1424 - (yi)-15), Point2i(2136 + (xi), 1424 - (yi)+15), Scalar(0, 0, 255), 2, LINE_AA, 0);
		circle(imgt, Point2i(2136 + (xi), 1424 - (yi)), 15, Scalar(0, 0, 255), 1, LINE_AA, 0);
	}
	imshow("iteration", imgt);
	waitKey(1200);
}

void RESECTION::updateX()
{
	//�����ֵ
	X = (A.t()*A).inv()*A.t()*L;
	cout << X.at<float>(0, 0) << endl;
	//����һ���ⷽԪ�س�Ա����
	Xs = Xs + X.at<float>(0, 0);
	Ys = Ys + X.at<float>(1, 0);
	Zs = Zs + X.at<float>(2, 0);
	Phi = Phi + X.at<float>(3, 0);
	Omega = Omega + X.at<float>(4, 0);
	Kappa = Kappa + X.at<float>(5, 0);
	f = f + X.at<float>(6, 0);
	x0 = x0 + X.at<float>(7, 0);
	y0 = y0 + X.at<float>(8, 0);
	k1 = k1 + X.at<float>(9, 0);
	k2 = k2 + X.at<float>(10, 0);
	p1 = p1 + X.at<float>(11, 0);
	p2 = p2 + X.at<float>(12, 0);
}

void RESECTION::writeResults()//������
{
	// ������
	V = A * X - L;
	// ���������
	Mat M0 = Mat(1, 1, CV_32FC1);
	M0 = (V.t()*V) / (float(equationNum) - float(elementNum)); //��������ʽ
	float m = M0.at<float>(0, 0);
	m = sqrt(m);//��λȨ�����
	//Э������
	Mat Q = Mat(elementNum, elementNum, CV_32FC1);
	Q = (A.t()*A).inv();
	//��Э������ĶԽ�Ԫ�����õ������
	Mat M = Mat(elementNum, 1, CV_32FC1);
	for (int i = 0; i < elementNum; i++)
	{
		M.at<float>(i, 0) = sqrt(Q.at<float>(i, i)*m);
	}
	string results_file_path = image_file_path;
	results_file_path.append("results.csv");
	ofstream ofs(results_file_path);
	if (!ofs)
	{
		cout << "Cannot Write Results" << endl;
		return;
	}
	ofs << "*******************************************************" << endl;
	ofs << "If all results is NaN, the notice in the line 118 of RESECTION.cpp may be a solution" << endl;
	ofs << "*******************************************************" << endl;
	ofs << "Number of iterations" << itrNum << endl;
	ofs << "MSE(mean square error) of unit weight" << m << "pixel" << endl;
	ofs << "Para Names,Results,MSE,Unit" << endl;
	ofs << "Xs," << Xs << "," << M.at<float>(0, 0) << "," << " mm" << endl;
	ofs << "Ys," << Ys << "," << M.at<float>(1, 0) << "," << " mm" << endl;
	ofs << "Zs," << Zs << "," << M.at<float>(2, 0) << "," << " mm" << endl;
	ofs << "Phi," << Phi << "," << M.at<float>(3, 0) << "," << " Rad" << endl;
	ofs << "Omega," << Omega << "," << M.at<float>(4, 0) << "," << " Rad" << endl;
	ofs << "Kappa," << Kappa << "," << M.at<float>(5, 0) << "," << " Rad" << endl;
	ofs << "f," << f << "," << M.at<float>(6, 0) << "," << " pixel" << endl;
	ofs << "x0," << x0 << "," << M.at<float>(7, 0) << "," << " pixel" << endl;
	ofs << "y0," << y0 << "," << M.at<float>(8, 0) << "," << " pixel" << endl;
	ofs << "k1," << k1 << "," << M.at<float>(9, 0) << "," << " (pixel)^(-2)" << endl;
	ofs << "k2," << k2 << "," << M.at<float>(10, 0) << "," << " (pixel)^(-4)" << endl;
	ofs << "p1," << p1 << "," << M.at<float>(11, 0) << "," << " (pixel)^(-1)" << endl;
	ofs << "p2," << p2 << "," << M.at<float>(12, 0) << "," << " (pixel)^(-1)" << endl;
	ofs << "*******************************************************" << endl;
	ofs << "Pt No.,x-res,y-res,unit is pixel" << endl;
	for (int i = 0; i < imagePtsNum; i++)
	{
		ofs << imagePts.at(i).ID << "," << V.at<float>(2 * i, 0) << "," << V.at<float>(2 * i + 1, 0) << endl;
	}
	ofs.close();
}