/*************************************************************************
    > File Name: BMPTransByOpenCV.cpp
    > Author: Jintao Yang
    > Mail: 18608842770@163.com 
    > Created Time: 2020年04月10日 星期五 18时42分24秒
 ************************************************************************/

// MIT License

// Copyright (c) 2020 - * Joker2770

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "BMPTransByOpenCV.h"
#include<unistd.h>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

//Mat g_srcImage, g_dstImage, g_tmpImage;
//-----------------------------------【命名空间声明部分】--------------------------------------
//		描述：包含程序所使用的命名空间
//----------------------------------------------------------------------------------------------- 
using namespace std;
using namespace cv;

//14byte文件头
typedef  struct  _tagBITMAPFILEHEADER
{
	unsigned short    	bfType;       		//位图文件的类型，必须为BM 
	unsigned int       bfSize;       		//文件大小，以字节为单位
	unsigned short		bfReserverd1; 		//位图文件保留字，必须为0 
	unsigned short		bfReserverd2; 		//位图文件保留字，必须为0 
	unsigned int       bfbfOffBits;  		//位图文件头到数据的偏移量，以字节为单位
} __attribute__((packed)) _BITMAPFILEHEADER;//使编译器不优化，其大小为14字节

//40byte信息头
typedef  struct  _tagBITMAPINFOHEADER
{
	unsigned int biSize;                        //该结构大小，字节为单位
	int  biWidth;                     			//图形宽度以象素为单位
	int  biHeight;                     			//图形高度以象素为单位
	unsigned short biPlanes;               		//目标设备的级别，必须为1 
	unsigned short biBitcount;             		//颜色深度，每个象素所需要的位数
	unsigned int biCompression;        			//位图的压缩类型
	unsigned int biSizeImage;              		//位图的大小，以字节为单位
	int  biXPelsPermeter;       				//位图水平分辨率，每米像素数
	int  biYPelsPermeter;       				//位图垂直分辨率，每米像素数
	unsigned int biClrUsed;            			//位图实际使用的颜色表中的颜色数
	unsigned int biClrImportant;       			//位图显示过程中重要的颜色数
} __attribute__((packed))  _BITMAPINFOHEADER;	//使编译器不优化，其大小为40字节

typedef  struct
{
	_BITMAPFILEHEADER  file; //文件信息区
	_BITMAPINFOHEADER  info; //图象信息区
} __attribute__((packed)) _bmp;


void write_monochrome_bitmap_file(const char* szSrcPath, const char *szDesPath)
{
	_bmp  m = { 0 };        //定义一个位图结构
	FILE *fp;

	IplImage * temp = cvLoadImage(szSrcPath, 0);

	fp = fopen(szDesPath, "wb+");
	if (NULL == fp)
	{
		printf("can't open the bmp imgae.\n");
		exit(0);
	}

	int widthStep = 0;
	widthStep = ((temp->width + 31) / 32) * 4;

	//写入文件头
	m.file.bfType = 0X4D42;
	m.file.bfSize = widthStep * temp->height + 62;       //文件大小，数据大小+文件头大小
	m.file.bfReserverd1 = 0;
	m.file.bfReserverd2 = 0;
	m.file.bfbfOffBits = 62;

	//写入信息头
	m.info.biSize = 40;
	m.info.biWidth = temp->width;
	m.info.biHeight = temp->height;
	m.info.biPlanes = 1;
	m.info.biBitcount = 1;
	m.info.biCompression = 0;
	m.info.biSizeImage = widthStep * temp->height;
	m.info.biXPelsPermeter = 0;
	m.info.biClrUsed = 0;
	m.info.biClrImportant = 0;

	fseek(fp, 0, SEEK_SET);
	fwrite(&(m.file.bfType), sizeof(m.file.bfType), 1, fp);
	fseek(fp, 2, SEEK_SET);
	fwrite(&(m.file.bfSize), sizeof(m) - 2, 1, fp);

	//54-62共八个字节表示调色板信息
	uchar Palette[8] = { 0,0,0,0,255,255,255,0 };//
	fseek(fp, 54, SEEK_SET);
	fwrite(Palette, sizeof(uchar), 8, fp);

	uchar* data = new uchar[widthStep*temp->height];
	memset(data, 0, widthStep*temp->height);

	//单色位图，1表示白色，0表示黑色，结果图背景为白（1），线条为黑（0）
	for (int i = 0; i < temp->height; i++)
	{
		for (int j = 0; j < widthStep; j++)
		{
			uchar temp_data = 0;
			for (int k = 0; k < 8; k++)
			{
				if (j * 8 + k < temp->width)
				{
					int temp_value = 0;
					//阈值设为150
					if ((uchar)temp->imageData[(temp->height - 1 - i)*temp->widthStep + j * 8 + k] > 150)
					{
						temp_value = 1 << (7 - k);
						temp_data += temp_value;
					}
				}
			}
			data[i*widthStep + j] = temp_data;
		}
	}

	fseek(fp, 62, SEEK_SET);
	fwrite(data, sizeof(uchar), widthStep*temp->height, fp);

	cvReleaseImage(&temp);
	temp = nullptr;
	delete[] data;
	data = NULL;
	fclose(fp);
	fp = NULL;
}


/****************************************
BMP_Zoom
功能说明：图片缩放
char *OldBmpPath：原始图片路径
char *NewBmpPath：处理后的图片路径
float fScale：缩放倍数
int beta：亮度，值越小打印越黑，彩色图片时建议都调为负数
int Transpose:旋转，0为不旋转，1,2,3分别对应90度，180度，270度
*****************************************/

BMPTRANSBYOPENCV_API int BMP_Zoom(char *OldBmpPath,char *NewBmpPath,float fScale)
{
	if(!OldBmpPath || !NewBmpPath)
	{
		return -1;
	}
    if(fScale == 0)
	{
		return -1;
	}


	if(0 != access(OldBmpPath, F_OK))
	{
		return -1;
	}

	//缩放
	char tempoldpicpath[1025] = "";

	strcpy(tempoldpicpath,OldBmpPath);



	//先对原图进行亮度调节
	Mat image = imread(/*OldBmpPath*/tempoldpicpath);
	Mat new_image = Mat::zeros( image.size(), image.type() );
	double alpha=1.0; /**< 控制对比度 */
	int beta_=-40;  /**< 控制亮度 */
	if (beta_>=0)
	{
		beta_=-40;
	}
	for( int y = 0; y < image.rows; y++ )
    {
        for( int x = 0; x < image.cols; x++ )
        {
            for( int c = 0; c < 3; c++ )
            {
                new_image.at<Vec3b>(y,x)[c] = saturate_cast<uchar>( alpha*( image.at<Vec3b>(y,x)[c] ) + beta_ );
            }
        }
    }

//	char sCfgPath1[1025] = "";
//	SEA_AUX_X::SAU_GetModuleCurPath(sCfgPath1);
//	strcat(sCfgPath1,"ClorPictur.bmp");

	Mat new_dest = Mat::zeros(new_image.size(), new_image.type());
	threshold(new_image, new_dest, 127, 255, THRESH_BINARY);	//根据阈值二值化
	imwrite("ClorPictur.bmp",new_dest);

	Mat g_srcImage, g_dstImage, g_tmpImage;

	//载入原图
	g_srcImage = imread("ClorPictur.bmp");
	if( !g_srcImage.data ) {return -1;}

	IplImage*pScr = nullptr;         //源图像指针    
    IplImage*dst = nullptr;          //目标图像指针    
	CvSize dst_cvsize;         //目标图像尺寸

	pScr=cvLoadImage("ClorPictur.bmp",0);

	dst_cvsize.width = (int)((pScr->width * fScale+0.05)/8)*8;  
    dst_cvsize.height = (int)((pScr->height * fScale+0.05)/8)*8; 

    dst = cvCreateImage(dst_cvsize,pScr->depth,pScr->nChannels);       //构造目标图象    

    cvResize(pScr, dst, CV_INTER_LINEAR);                              //缩放源图像到目标图像 

	//保存
	cvSaveImage("Tmp8bitPictur.bmp", dst);
 
    cvReleaseImage(&pScr);             //释放源图像占用的内存    
    cvReleaseImage(&dst);              //释放目标图像占用的内存   
	image.release();
	new_image.release();
	new_dest.release();
	g_srcImage.release();
	g_dstImage.release();
	g_tmpImage.release();

	pScr = nullptr;
	dst = nullptr;

	//保存单色位图
	//Win32Bitmap(NewBmpPath);
	write_monochrome_bitmap_file("Tmp8bitPictur.bmp", NewBmpPath);

	return 0;
}


/****************************************
BMP_Zoom
功能说明：图片缩放
char *OldBmpPath：原始图片路径
char *NewBmpPath：处理后的图片路径
float fScale：缩放倍数
int beta：亮度，值越小打印越黑，彩色图片时建议都调为负数
int Transpose:旋转，0为不旋转，1,2,3分别对应90度，180度，270度
*****************************************/

BMPTRANSBYOPENCV_API int BMP_ZoomEX(char *OldBmpPath,char *NewBmpPath,float fScale,int beta,int Transpose)
{
	if(!OldBmpPath || !NewBmpPath)
	{
		return -1;
	}
	if(fScale == 0)
	{
		return -1;
	}


	if(0 != access(OldBmpPath, F_OK))
	{
		return -1;
	}

	//缩放
	char tempoldpath[2048]="";
	char tempoldpicpath[1025] = "";

	double dscalar=0;

	if (Transpose>0 && Transpose<4)
	{
		strcpy(tempoldpath,OldBmpPath);
		IplImage* src = cvLoadImage(tempoldpath,-1);
		IplImage* srcCopy = cvCreateImage(cvSize(src->height,src->width),src->depth,src->nChannels);
		cvTranspose(src,srcCopy);
		cvFlip(srcCopy,NULL,Transpose);
		//cvNamedWindow("result");
		//cvShowImage("result",srcCopy);
		CvScalar scalar= cvAvg(srcCopy);
		dscalar=scalar.val[0];



//		SEA_AUX_X::SAU_GetModuleCurPath(tempoldpicpath);
		strcpy(tempoldpicpath,"TranPictur.bmp");

		cvSaveImage(tempoldpicpath,srcCopy);
		cvWaitKey();

		cvReleaseImage(&src);             //释放源图像占用的内存    
		cvReleaseImage(&srcCopy);              //释放目标图像占用的内存
	}
	else
	{
		strcpy(tempoldpicpath,OldBmpPath);
	}




	//先对原图进行亮度调节
	Mat image = imread(/*OldBmpPath*/tempoldpicpath);
	Mat new_image = Mat::zeros( image.size(), image.type() );
	double alpha=1.0; /**< 控制对比度 */
	double beta_=(dscalar-170);  /**< 控制亮度 */
	if (dscalar<155)
	{
		beta_=(dscalar-130);
	}

	for( int y = 0; y < image.rows; y++ )
	{
		for( int x = 0; x < image.cols; x++ )
		{
			for( int c = 0; c < 3; c++ )
			{
				new_image.at<Vec3b>(y,x)[c] = saturate_cast<uchar>( alpha*( image.at<Vec3b>(y,x)[c] ) + beta_ );
			}
		}
	}

	threshold(new_image, new_image, dscalar-50, 255, 3);


//	char sCfgPath1[1025] = "";
//	SEA_AUX_X::SAU_GetModuleCurPath(sCfgPath1);
//	strcat(sCfgPath1,"ClorPictur.bmp");

	Mat new_dest = Mat::zeros(new_image.size(), new_image.type());
	threshold(new_image, new_dest, 127, 255, CV_THRESH_BINARY);	//根据阈值二值化
	imwrite("ClorPictur.bmp", new_dest);

	Mat g_srcImage, g_dstImage, g_tmpImage;
	//载入原图
	g_srcImage = imread("ClorPictur.bmp");//工程目录下需要有一张名为1.jpg的测试图像，且其尺寸需被2的N次方整除，N为可以缩放的次数
	if( !g_srcImage.data ) {return -1;}

	IplImage*pScr = nullptr;         //源图像指针    
	IplImage*dst = nullptr;          //目标图像指针    
	CvSize dst_cvsize;         //目标图像尺寸


	pScr=cvLoadImage("ClorPictur.bmp",0);

	dst_cvsize.width = (int)((pScr->width * fScale + 0.05) / 8) * 8;
	dst_cvsize.height = (int)((pScr->height * fScale + 0.05) / 8) * 8;

	dst = cvCreateImage(dst_cvsize,pScr->depth,pScr->nChannels);       //构造目标图象    




	cvResize(pScr, dst, CV_INTER_LINEAR);                              //缩放源图像到目标图像 




	//保存
	cvSaveImage("Tmp8bitPictur.bmp", dst);

	image.release();
	new_image.release();
	new_dest.release();
	g_srcImage.release();
	g_dstImage.release();
	g_tmpImage.release();
	cvReleaseImage(&pScr);             //释放源图像占用的内存    
	cvReleaseImage(&dst);              //释放目标图像占用的内存    

	pScr = nullptr;
	dst = nullptr;

	//保存单色位图
	//Win32Bitmap(NewBmpPath);
	write_monochrome_bitmap_file("Tmp8bitPictur.bmp", NewBmpPath);

	return 0;
}


BMPTRANSBYOPENCV_API int GetBmpInfo(char *OldBmpPath,int *picwidth,int *picheight)
{
	if(!OldBmpPath)
	{
		return -1;
	}

	if(0 != access(OldBmpPath, F_OK))
	{
		return -1;
	}

	Mat g_srcImage;
	//载入原图
	string stroldbmppath=OldBmpPath;
	g_srcImage = imread(stroldbmppath);//工程目录下需要有一张名为1.jpg的测试图像，且其尺寸需被2的N次方整除，N为可以缩放的次数
	if( !g_srcImage.data ) 
	{
		return -1;
	}

	IplImage*pScr = 0;         //源图像指针      
	//CvSize dst_cvsize;         //目标图像尺寸

	pScr=cvLoadImage(OldBmpPath,1);  

	*picwidth=pScr->width;
	*picheight=pScr->height;

	g_srcImage.release();
	cvReleaseImage(&pScr);             //释放源图像占用的内存  

	
	return 0;
}