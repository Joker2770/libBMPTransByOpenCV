/*************************************************************************
    > File Name: main.cpp
    > Author: Jintao Yang
    > Mail: 18608842770@163.com 
    > Created Time: 2020年04月10日 星期五 15时45分24秒
 ************************************************************************/

#include<dlfcn.h>
#include<iostream>
using namespace std;

typedef int (*pGetBmpInfo)(char *OldBmpPath,int *picwidth,int *picheight);
typedef int (*pBmp_Zoom)(char *OldBmpPath,char *NewBmpPath,float fScale);

int main(int argc, char* argv[])
{
	pGetBmpInfo pGetBmpInfo_ = NULL;
	pBmp_Zoom pBmp_Zoom_ = NULL;
	
	void *dlhandle = NULL;

	dlhandle = dlopen("./libBMPTransByOpenCV.so", RTLD_LAZY);
	if(!dlhandle)
	{
		cout << "dlhandle == NULL!" << endl;
		return -2;
	}

	pGetBmpInfo_ = (pGetBmpInfo)dlsym(dlhandle, "GetBmpInfo");
	if(!pGetBmpInfo_)
	{
		if(dlhandle)
		{
			dlclose(dlhandle);
			dlhandle = NULL;
		}
		cout << "pGetBmpInfo_ == NULL!" << endl;
		return -1;
	}
	pBmp_Zoom_ = (pBmp_Zoom)dlsym(dlhandle, "BMP_Zoom");
	if(!pBmp_Zoom_)
	{
		if(dlhandle)
		{
			dlclose(dlhandle);
			dlhandle = NULL;
		}
		cout << "pBmp_Zoom_ == NULL!" << endl;
		return -1;
	}

	int iwidth = 0, iheight = 0;
	int iret = pGetBmpInfo_(argv[1], &iwidth, &iheight);
	if(0 != iret)
	{
		if(dlhandle)
		{
			dlclose(dlhandle);
			dlhandle = NULL;
		}
		cout << "pGetBmpInfo_ exec err!" << endl;
		return -3;
	}
	
	cout << "width: " << iwidth << endl;
	cout << "height: " << iheight << endl;

	char sDes[32] = "dest.bmp";
	iret = pBmp_Zoom_(argv[1], sDes, 1.0);
	if(0 != iret)
	{
		if(dlhandle)
		{
			dlclose(dlhandle);
			dlhandle = NULL;
		}
		cout << "pBmp_Zoom_ exec err!" << endl;
		return -3;
	}

	iret = pGetBmpInfo_(sDes, &iwidth, &iheight);
	if(0 != iret)
	{
		if(dlhandle)
		{
			dlclose(dlhandle);
			dlhandle = NULL;
		}
		cout << "pGetBmpInfo_ exec err!" << endl;
		return -3;
	}
	
	cout << "dest_width: " << iwidth << endl;
	cout << "dest_height: " << iheight << endl;

	pGetBmpInfo_ = NULL;
	pBmp_Zoom_ = NULL;

	return 0;
}

