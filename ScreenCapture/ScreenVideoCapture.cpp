#include "ScreenVideoCapture.h"

void ScreenVideoCapture::init(HWND window) { 
	this->targetWindow = window;
	if (this->targetWindow == NULL)
		return;
	hwindowDC = GetDC(targetWindow);
	if (this->targetWindow == NULL)
		return;
	GetClientRect(targetWindow, &this->captureArea);
	hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
	SetStretchBltMode(hwindowCompatibleDC, STRETCH_ANDSCANS);

	// Initialize a bitmap
	int width = this->captureArea.right - this->captureArea.left;
	int height = this->captureArea.bottom - this->captureArea.top;
	hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	// The negative height is required -- removing the inversion will make the image appear upside-down.
	bi.biHeight = 0 - height;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;
}
void ScreenVideoCapture::release() {
	// Clean up memory to avoid leaks
	if (this->targetWindow != NULL) {
		DeleteObject(this->hbwindow);
		DeleteDC(this->hwindowCompatibleDC);
		ReleaseDC(this->targetWindow, hwindowDC);
		this->captureArea.left = 
		this->captureArea.right = 
		this->captureArea.bottom = 
		this->captureArea.top = 0;
	}
}

void ScreenVideoCapture::read(cv::Mat& destination) {
	if (targetWindow == NULL)
		throw new std::exception("No target monitor specified! The 'open()' method must be called to select a target monitor before frames can be read.");

	captureHwnd(destination);
}

ScreenVideoCapture& ScreenVideoCapture::operator>>(cv::Mat& destination) {
	read(destination);
	return *this;
}

void ScreenVideoCapture::captureHwnd(cv::Mat& dest) {
	int width = this->captureArea.right - this->captureArea.left;
	int height = this->captureArea.bottom - this->captureArea.top;
	if (width == 0 || height == 0)
		return;
	dest.create(height, width, CV_8UC4);


	SelectObject(hwindowCompatibleDC, hbwindow);
	// Copy from the window device context to the bitmap device context
	// Use BitBlt to do a copy without any stretching -- the output is of the same dimensions as the target area.

	BitBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, SRCPAINT);

	PrintWindow(targetWindow, hwindowCompatibleDC, 0);

	// Copy into our own buffer as device-independent bitmap
	GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, dest.data, (BITMAPINFO *)&bi, DIB_RGB_COLORS);
}