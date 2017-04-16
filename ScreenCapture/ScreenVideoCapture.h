#pragma once

#include <opencv2\opencv.hpp>
#include <Windows.h>

class ScreenVideoCapture {
public:
	ScreenVideoCapture(){};
	~ScreenVideoCapture(){};
	void read(cv::Mat& destination);
	void init(HWND hwnd); 
	void release();
	ScreenVideoCapture& operator>>(cv::Mat& destination);

private:
	RECT captureArea;
	HWND targetWindow;
	HDC hwindowDC, hwindowCompatibleDC;
	HBITMAP hbwindow;
	BITMAPINFOHEADER  bi;
	void captureHwnd(cv::Mat& dest);
};

struct MonitorIndexLookupInfo {
	int targetIndex;

	RECT outRect;
	int currentIndex;
};