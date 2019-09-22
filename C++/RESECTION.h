#pragma once
#include"pch.h"
#define DEG2RAD 0.01745329
using namespace std;
using namespace cv;

/****************************************************************/
//���ڿ��Ƴ� �� ��������
//���Ƶ����꣺������Ƴ���X��ǰ��Y���ң�Z���ϡ�����������ϵ��
//������꣺����ͼ��x���ң�y���£�z��ǰ������������ϵ��
/****************************************************************/
/****************************************************************/
//
//�ռ�󷽽���
//
//����ʽ�� V = At + CX2 + DX - L
//���壺
//	A/C/D��ϵ����
//	t���ⷽλԪ�صĸ�����
//	X2���ڷ�λԪ�صĸ�����
//	X������ϵ���ĸ�����
//	L�����۲�ֵ - ����ֵ������ֵ��������ģ�
//
/****************************************************************/

class RESECTION
{
public:
	bool disp = false;//�Ƿ����������Ϣ
	string control_file_path;
	string image_file_path;
	string image;
	//���Ը���ʵ�����������������޸�RESECTION����Ĳ�����ֵ
	double Xs;	//�ⷽλ��Ԫ��Xs��ֵ
	double Ys;	//�ⷽλ��Ԫ��Ys��ֵ
	double Zs;	//�ⷽλ��Ԫ��Zs��ֵ
	double Phi;	//�ⷽλ��Ԫ��Phi��ֵ
	double Omega;//�ⷽλ��Ԫ��Omega��ֵ
	double Kappa;//�ⷽλ��Ԫ��Omega��ֵ
	double x0;	//�������x0��ֵ
	double y0;	//�������y0��ֵ
	double f;	//�����ֵ
	double k1;	//�������k1��ֵ
	double k2;	//�������k2��ֵ
	double p1;	//�������p1��ֵ
	double p2;	//�������p1��ֵ

private:
	Mat img;			//�洢ͼ��
	Mat controlPts;			  //[ID,X,Y,Z]
	vector<ImagePt> imagePts; //[ID,x,y,X,Y,Z]
	int imagePtsNum;	//������
	int equationNum;	//���̵ĸ�����ÿ������x��y�ֱ���һ�����̣�����������
	int elementNum;		//δ֪������������ʵ����Ϊ13��
	Mat R;				//��ת����
						// [V = At + CX2 + DX - L] ==> [V=AX-L]
	Mat V;				//һ������Ӧ��������
	Mat A;				//A/C/Dϵ�������һ������
	Mat X;				//���е�δ֪����һ������
	Mat L;
	int itrNum;			//��������
public:
	RESECTION(string control_file_path, string image_file_path, string image);
	~RESECTION();
	void Resection();
private:
	int readControlPts(string control_file_path);//���ܣ���ȡ���Ƶ������ļ�
	int readImagePts(string image_file_path);//���ܣ���ȡ��������ļ�
	void updateRAL();//���¼������ R\A\L
	void updateX();
	void writeResults();//������
};
