#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <io.h> // for getFiles
#include <opencv2/opencv.hpp>
#include "ImageProcess.h"
//#include "preProcess.h"
#include "ostuRemoveBorder.h"
using namespace std;


void getFiles( string path, vector<string>& files )
{
	//文件句柄
	long   hFile   =   0;
	//文件信息
	struct _finddata_t fileinfo;
	string p;
	if((hFile = _findfirst(p.assign(path).append("\\*").c_str(),&fileinfo)) !=  -1)
	{
		do
		{
			//如果是目录,迭代之
			//如果不是,加入列表
			if((fileinfo.attrib &  _A_SUBDIR))
			{
				if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)
					getFiles( p.assign(path).append("\\").append(fileinfo.name), files );
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name) );
			}
		}while(_findnext(hFile, &fileinfo)  == 0);
		_findclose(hFile);
	}
}

string splitFileName(const string& file) {
	size_t found = file.find_last_of("/\\");
	return file.substr(found + 1);
}

//int main()
//{
//	string newFilePath = "..\\..\\target";
//
//	string rm = "rd " + newFilePath;
//	system(rm.c_str());
//	string mkdir = "md " + newFilePath;
//	system(mkdir.c_str());
//
//	string filePath = "D:\\Projects\\ImageSearch\\BrandSearch\\target";
//	vector<string> files;
//	getFiles(filePath, files);
//	cout << "Input Read End." << endl;
//
//	size_t n = files.size();
//	for (size_t i = 0; i < n; ++i) {
//		string inPath = files[i];
//		string fileName = splitFileName(files[i]);
//		string outPath = newFilePath + "\\" + fileName;
//
//		IplImage *img = cvLoadImage((inPath.c_str()), CV_LOAD_IMAGE_COLOR);
//		IplImage *GrayImage = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);
//		cvCvtColor(img,GrayImage,CV_RGB2GRAY);
//
//		//int ostuThreshold = GetOtsuThreshold(GrayImage);
//		int ostuThreshold = 245;
//		SegPosition seg_position(0, 0, GrayImage -> width - 1, GrayImage -> height - 1);
//		GetsegPosition(GrayImage, ostuThreshold, seg_position);
//		IplImage *validPart = extractValidPart(img, seg_position);
//
//		//归一化图片大小
//		IplImage* stdImg= cvCreateImage( cvSize(240,260), IPL_DEPTH_8U, 3);
//		cvResize(validPart, stdImg, CV_INTER_LINEAR);
//		cvSaveImage(outPath.c_str(), stdImg);
//
//		cout << outPath << endl;
//
//		cvReleaseImage(&validPart);
//		cvReleaseImage(&GrayImage);
//		cvReleaseImage(&img);
//	}
//	system("pause");
//	return 0;
//}
//
int main() 
{
	// 存放 
		string newFilePath = "..\\..\\8";
	

		string rm = "rd " + newFilePath;
		system(rm.c_str());
		string mkdir = "md " + newFilePath;
		system(mkdir.c_str());
	
		//原图
		//string filePath = "E:\\AnXingle\\WorkSpace\\Test_Picture\\1.3.1 朝阳或夕阳";
		//string filePath = "E:\\AnXingle\\WorkSpace\\Test_Picture\\2.1.20 骑或伴随着马,骡,驴的男人";
		//string filePath = "E:\\AnXingle\\WorkSpace\\Test_Picture\\3.7.1 鹰,隼,秃鹫";
		//string filePath = "E:\\AnXingle\\WorkSpace\\Test_Picture\\3.11.9 短吻鳄鱼,大鳄鱼,鳄鱼";
		//string filePath = "E:\\AnXingle\\WorkSpace\\Test_Picture\\5.9.24 各种不同种类蔬菜的组合";
		//string filePath = "E:\\AnXingle\\WorkSpace\\Test_Picture\\5.11.1 其它植物";
		//string filePath = "E:\\AnXingle\\WorkSpace\\Test_Picture\\6.1.2 山,山景";
		string filePath = "E:\\AnXingle\\WorkSpace\\Test_Picture\\26.2.7 半圆,半个椭圆";
		//string filePath = "D:\\Users\\Desktop\\1\\circle";
		vector<string> files;
		getFiles(filePath, files);
		cout << "Input Read End." << endl;

		size_t n = files.size();
		for (size_t i = 0; i < n; ++i) {
			string inPath = files[i];
			string fileName = splitFileName(files[i]);
			cout << fileName << endl;
			string outPath = newFilePath + "\\" + fileName;
			//string outPath = filePath + "\\" + fileName.substr(0, fileName.find_last_of(".")) + "_re.jpg";
			Mat img = imread(inPath.c_str());
			//Mat re = preProcess(img);
			Mat re = ostuRemoveBorder(img);

			resize(re, re, Size(240, 260), 0, 0, CV_INTER_LINEAR);
			imwrite(outPath.c_str(),re);
			cout << outPath << endl;
		}
		system("pause");
		return 0;
}