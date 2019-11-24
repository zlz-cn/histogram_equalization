#include <iostream>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <windows.h>
#include <Commdlg.h>
#include <stdio.h>
#include<exception>

OPENFILENAME ofn;
char szFile[300];

char* ConvertLPWSTRToLPSTR(LPWSTR lpwszStrIn)
{
	LPSTR pszOut = NULL;
	try
	{
		if (lpwszStrIn != NULL)
		{
			int nInputStrLen = wcslen(lpwszStrIn);
			int nOutputStrLen = WideCharToMultiByte(CP_ACP, 0, lpwszStrIn, nInputStrLen, NULL, 0, 0, 0) + 2;
			pszOut = new char[nOutputStrLen];

			if (pszOut)
			{
				memset(pszOut, 0x00, nOutputStrLen);
				WideCharToMultiByte(CP_ACP, 0, lpwszStrIn, nInputStrLen, pszOut, nOutputStrLen, 0, 0);
			}
		}
	}
	catch (std::exception e)
	{
	}

	return pszOut;
}

char* flashreplace(char* file) {
	int size = strlen(file);
	for (int i = 0; i <= size; i++) {
		if (*(file + i) == '\\')
		{
			*(file + i) = '/';
		}
	}
	for (int i = 0; i <= size; i++) {
		printf("%c", *(file + i));
	}


	return file;

}

char* getfile() {
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = (LPWSTR)szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (GetOpenFileName(&ofn))
	{
		//wprintf(L"%s\n", ofn.lpstrFile);
		char* getf = ConvertLPWSTRToLPSTR(ofn.lpstrFile);
		getf = flashreplace(getf);
		return getf;
	}
	else
	{
		printf("user cancelled\n");
	}
}

void Histogram_equalization(cv::Mat& src, cv::Mat& dst) {
	CV_Assert(src.depth() == CV_8U);
	src.copyTo(dst);
	int nr = src.rows;
	int nc = src.cols;
	int pixnum = nr * nc;
	if (src.channels() == 1) {
		//统计直方图
		int gray[256] = { 0 };
		for (int i = 1; i < nr; ++i) {
			const uchar* ptr = src.ptr<uchar>(i);
			for (int j = 0; j < nc; ++j) {
				gray[ptr[j]]++;
			}
		}
		//计算分布函数
		int LUT[256];
		int sum = 0;
		for (int k = 0; k < 256; k++) {
			sum = sum + gray[k];
			LUT[k] = 255 * sum / pixnum;
		}
		//灰度变换（赋值）
		for (int i = 1; i < nr; ++i) {
			const uchar* ptr_src = src.ptr<uchar>(i);
			uchar* ptr_dst = dst.ptr<uchar>(i);
			for (int j = 0; j < nc; ++j) {
				ptr_dst[j] = LUT[ptr_src[j]];
			}
		}
	}
	else {
		//统计直方图
		int B[256] = { 0 };
		int G[256] = { 0 };
		int R[256] = { 0 };
		for (int i = 0; i < nr; ++i) {
			for (int j = 0; j < nc; ++j) {
				B[src.at<cv::Vec3b>(i, j)[0]]++;
				G[src.at<cv::Vec3b>(i, j)[1]]++;
				R[src.at<cv::Vec3b>(i, j)[2]]++;
			}
		}
		//计算分布函数
		int LUT_B[256], LUT_G[256], LUT_R[256];
		int sum_B = 0, sum_G = 0, sum_R = 0;
		for (int k = 0; k < 256; k++) {
			sum_B = sum_B + B[k];
			sum_G = sum_G + G[k];
			sum_R = sum_R + R[k];
			LUT_B[k] = 255 * sum_B / pixnum;
			LUT_G[k] = 255 * sum_G / pixnum;
			LUT_R[k] = 255 * sum_R / pixnum;
		}
		//灰度变换（赋值）
		for (int i = 0; i < nr; ++i) {
			for (int j = 0; j < nc; ++j) {
				dst.at<cv::Vec3b>(i, j)[0] = LUT_B[src.at<cv::Vec3b>(i, j)[0]];
				dst.at<cv::Vec3b>(i, j)[1] = LUT_G[src.at<cv::Vec3b>(i, j)[1]];
				dst.at<cv::Vec3b>(i, j)[2] = LUT_R[src.at<cv::Vec3b>(i, j)[2]];
			}
		}
	}
}

int main() {
	char* opfile = getfile();
	cv::Mat src = cv::imread(opfile);
	if (src.empty()) {
		return -1;
	}
	cvtColor(src, src, CV_RGB2GRAY);
	cv::Mat dst;

	Histogram_equalization(src, dst);//直方图均衡

	cv::namedWindow("src",0);
	cv::imshow("src", src);
	cv::namedWindow("dst",0);
	cv::imshow("dst", dst);
	cv::waitKey(0);

}
