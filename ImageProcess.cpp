#include "ImageProcess.h"

//u=w0*u0+w1*u1。从最小灰度值到最大灰度值遍历t，当t使得值g=w0*(u0-u)2+w1*(u1-u)2 

int GetOtsuThreshold(const IplImage *frame) //大津法求阈值  
{  
	const int GrayScale = 256;    
	int width = frame->width;  
	int height = frame->height; 
	int step  = frame->widthStep;
	// 像素频数
	int pixelCount[GrayScale]={0};
	// 像素概率
	float pixelPro[GrayScale]={0};  
	int pixelSum = width * height;
	uchar* data = (uchar*)frame->imageData;  

	//统计每个灰度级中像素的个数  
	for(int i = 0; i < height; i++)  
	{  
		for(int j = 0;j < width;j++)  
		{  
			int bin = (int)data[i * step + j];
			pixelCount[bin]++;  
		}  
	}  

	//计算每个灰度级的像素数目占整幅图像的比例  
	for(int i = 0; i < GrayScale; i++)  
	{  
		pixelPro[i] = (float)pixelCount[i] / pixelSum;  
	}  

	//遍历灰度级[0,255],寻找合适的threshold  
	float w0, w1, u0tmp, u1tmp, u0, u1, deltaTmp;
	float deltaMax = 0;
	int threshold = 0;
	// 遍历阈值t
	for(int i = 0; i < GrayScale; i++)  
	{  
		w0 = w1 = u0tmp = u1tmp = u0 = u1 = deltaTmp = 0;  
		for(int j = 0; j < GrayScale; j++)  
		{  
			if(j <= i)   //背景部分  
			{  
				w0 += pixelPro[j];  
				u0tmp += j * pixelPro[j];  
			}  
			else   //前景部分  
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
	//距离白色很近的像素数目占一列（或一行）像素总数目的比例
	const float ratio_pixel_like_white = 0.95f;


	//-----------------按列扫描-----------------------------------
	//切割的最左边x坐标
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
	//切割的最右边x坐标
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


	//-----------------按行扫描，----------------------------------
	//切割的最上y坐标
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
	//切割的最下y坐标
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
	//防止过分割
	if(seg_position.top_left_x > 0.5*width) {seg_position.top_left_x = (int)(width/3);}
	if(seg_position.top_left_y > 0.5*height) {seg_position.top_left_y = (int)(height/3);}
	if(seg_position.down_right_x < 0.5*width) {seg_position.down_right_x = (int)(width*2/3);}
	if(seg_position.down_right_y < 0.5*height) {seg_position.down_right_y = (int)(height*2/3);}
	//释放
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