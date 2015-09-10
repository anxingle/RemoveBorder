#include "ImageProcess.h"

//u=w0*u0+w1*u1������С�Ҷ�ֵ�����Ҷ�ֵ����t����tʹ��ֵg=w0*(u0-u)2+w1*(u1-u)2 

int GetOtsuThreshold(const IplImage *frame) //�������ֵ  
{  
	const int GrayScale = 256;    
	int width = frame->width;  
	int height = frame->height; 
	int step  = frame->widthStep;
	// ����Ƶ��
	int pixelCount[GrayScale]={0};
	// ���ظ���
	float pixelPro[GrayScale]={0};  
	int pixelSum = width * height;
	uchar* data = (uchar*)frame->imageData;  

	//ͳ��ÿ���Ҷȼ������صĸ���  
	for(int i = 0; i < height; i++)  
	{  
		for(int j = 0;j < width;j++)  
		{  
			int bin = (int)data[i * step + j];
			pixelCount[bin]++;  
		}  
	}  

	//����ÿ���Ҷȼ���������Ŀռ����ͼ��ı���  
	for(int i = 0; i < GrayScale; i++)  
	{  
		pixelPro[i] = (float)pixelCount[i] / pixelSum;  
	}  

	//�����Ҷȼ�[0,255],Ѱ�Һ��ʵ�threshold  
	float w0, w1, u0tmp, u1tmp, u0, u1, deltaTmp;
	float deltaMax = 0;
	int threshold = 0;
	// ������ֵt
	for(int i = 0; i < GrayScale; i++)  
	{  
		w0 = w1 = u0tmp = u1tmp = u0 = u1 = deltaTmp = 0;  
		for(int j = 0; j < GrayScale; j++)  
		{  
			if(j <= i)   //��������  
			{  
				w0 += pixelPro[j];  
				u0tmp += j * pixelPro[j];  
			}  
			else   //ǰ������  
			{  
				w1 += pixelPro[j];  
				u1tmp += j * pixelPro[j];  
			}  
		}  
		u0 = u0tmp / w0;  
		u1 = u1tmp / w1;  
		deltaTmp = (float)(w0 *w1* pow((u0 - u1), 2)) ;  
		if(deltaTmp > deltaMax)  
		{  
			deltaMax = deltaTmp;  
			threshold = i;  
		}  
	}  
	return threshold;  
}  

void GetsegPosition(const IplImage *img, int threshold, SegPosition& seg_position)
{
	int height = img->height;
	int width = img->width;

	CvMat* imgMat = cvCreateMat(height, width, CV_64FC1);
	cvConvert(img, imgMat);
	CvMat* cmpResult = cvCreateMat(height, width, CV_8UC1);
	cvCmpS(imgMat, threshold, cmpResult, CV_CMP_GT);
	//�����ɫ�ܽ���������Ŀռһ�У���һ�У���������Ŀ�ı���
	const float ratio_pixel_like_white = 0.95f;


	//-----------------����ɨ��-----------------------------------
	//�и�������x����
	for(int col = 0; col < width; col++)
	{
		CvMat* colMat = cvCreateMat(height, 1, CV_8UC1);
		cvGetCol(cmpResult, colMat, col);
		int nonZero = cvCountNonZero(colMat);
		if(nonZero >= height*ratio_pixel_like_white)
		{
			seg_position.top_left_x = col;
		}
		else
		{
			cvReleaseMat(&colMat);
			break;
		}
		cvReleaseMat(&colMat);
	}
	//�и�����ұ�x����
	for(int col = width-1; col >= 0; col--)
	{
		CvMat* colMat = cvCreateMat(height, 1, CV_8UC1);
		cvGetCol(cmpResult, colMat, col);
		int nonZero = cvCountNonZero(colMat);
		if(nonZero >= height*ratio_pixel_like_white)
		{
			seg_position.down_right_x = col;
		}
		else
		{
			cvReleaseMat(&colMat);
			break;
		}
		cvReleaseMat(&colMat);
	}


	//-----------------����ɨ�裬----------------------------------
	//�и������y����
	for(int row = 0; row < height; row++)
	{
		CvMat* rowMat = cvCreateMat(1, width, CV_8UC1);
		cvGetRow(cmpResult, rowMat, row);
		int nonZero = cvCountNonZero(rowMat);
		if(nonZero >= width*ratio_pixel_like_white)
		{
			seg_position.top_left_y = row;
		}
		else
		{
			cvReleaseMat(&rowMat);
			break;
		}
		cvReleaseMat(&rowMat);
	}
	//�и������y����
	for(int row = height - 1; row >= 0; row--)
	{
		CvMat* rowMat = cvCreateMat(1, width, CV_8UC1);
		cvGetRow(cmpResult, rowMat, row);
		int nonZero = cvCountNonZero(rowMat);
		if(nonZero >= width*ratio_pixel_like_white)
		{
			seg_position.down_right_y = row;
		}
		else
		{
			cvReleaseMat(&rowMat);
			break;
		}
		cvReleaseMat(&rowMat);
	}
	//��ֹ���ָ�
	if(seg_position.top_left_x > 0.5*width) {seg_position.top_left_x = (int)(width/3);}
	if(seg_position.top_left_y > 0.5*height) {seg_position.top_left_y = (int)(height/3);}
	if(seg_position.down_right_x < 0.5*width) {seg_position.down_right_x = (int)(width*2/3);}
	if(seg_position.down_right_y < 0.5*height) {seg_position.down_right_y = (int)(height*2/3);}
	//�ͷ�
	cvReleaseMat(&cmpResult);
	cvReleaseMat(&imgMat);
}

IplImage* extractValidPart(IplImage *img_color, SegPosition& seg_position) {
	IplImage* dst= cvCreateImage( cvSize(seg_position.down_right_x - seg_position.top_left_x, seg_position.down_right_y - seg_position.top_left_y), img_color->depth, img_color->nChannels);
	cvSetImageROI(img_color, cvRect(seg_position.top_left_x, seg_position.top_left_y, seg_position.down_right_x - seg_position.top_left_x, seg_position.down_right_y - seg_position.top_left_y));
	cvCopy(img_color, dst);
	cvResetImageROI(img_color);
	return dst;
}