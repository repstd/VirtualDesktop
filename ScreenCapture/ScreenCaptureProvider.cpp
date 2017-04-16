#include "ScreenCaptureProvider.h"
#include <Windows.h>
#include <iostream>
#include <memory>
ScreenCaptureProvider* ScreenCaptureProvider::getInstance() {
	static ScreenCaptureProvider inst;
	return &inst;
}

ScreenCaptureProvider::ScreenCaptureProvider() {
	this->mCapture.reset(new ScreenVideoCapture());
	this->mSleepTime = 1000 / 60.0;
	this->mMutex = std::make_shared<OpenThreads::Mutex>(OpenThreads::Mutex::MUTEX_RECURSIVE);
}

ScreenCaptureProvider::~ScreenCaptureProvider() {
	this->mCapture.reset();
}
void ScreenCaptureProvider::registerCallback(ScreenCaptureCallback* callback) {
	this->mCallback = std::shared_ptr<ScreenCaptureCallback>(callback);
}
void ScreenCaptureProvider::unregisterCallback() {
	this->mCallback.reset();
}
void ScreenCaptureProvider::startCapture(float fps) {
	if (fps != -1 && fps != 0) {
		this->mSleepTime = 1000.0 / fps;
	}
	while (true) {
		captureSync(false);
	}
}
std::vector<std::shared_ptr<ScreenCaptureWorker>> result;
std::vector<std::shared_ptr<ScreenCaptureWorker>>& ScreenCaptureProvider::startCaptureAsyncTask(float fps, int count) {
	if (fps != -1 && fps != 0) {
		this->mSleepTime = 1000.0 / fps;
	}
	for (int i = 0; i < count; i++) {
		std::shared_ptr<ScreenCaptureWorker> worker = std::make_shared<ScreenCaptureWorker>();
		worker->start();
		result.push_back(worker);
	}
	return result;
}
void ScreenCaptureProvider::startCaptureAsync(float fps) {
	startCaptureAsyncTask(fps, 1);
}
void ScreenCaptureProvider::frame() {
	ScreenCaptureProvider::getInstance()->captureSync(false);
}
void ScreenCaptureWorker::run() {
	while (true) {
		try {
			ScreenCaptureProvider::getInstance()->captureSync(true);
		}
		catch (const std::exception& e) {
			std::cout << e.what() << std::endl;
			break;
		}
	}
}
void ScreenCaptureProvider::captureSync(bool lock) {
	if (lock) {
		this->mMutex->lock();
	}
	CacheItemList cache;
	EnumWindows(mWindowFilter, (LPARAM)(&cache));
	if (this->mCallback != nullptr&&this->mCallback.get() != nullptr) {
		this->mCallback->onCapture(cache);
	}
	Sleep(mSleepTime);
	if (lock) {
		this->mMutex->unlock();
	}
}
void ScreenCaptureProvider::addToCache(CacheItemList& cache, HWND hwnd) {
	std::shared_ptr<CacheItem> item(new CacheItem());
	char realName[196];
	GetWindowText(hwnd, realName, sizeof(realName));
	if (strlen(realName) == 0 || strstr(realName, "VD_") != NULL || strstr(realName, "ScreenCapture") != NULL) {
		return;
	}
	sprintf(item->mTitle, "VD_%s", realName);
	this->mCapture->init(hwnd);
	*(mCapture) >> (item->mImage);
	this->mCapture->release();
	if (item->mImage.rows == 0 || item->mImage.cols == 0)
		return;
	RECT rect;
	GetWindowRect(hwnd, &rect);
	item->mHwnd = hwnd;
	item->mX = rect.left;
	item->mY = rect.top;
	cache.push_back(item);
}
BOOL CALLBACK ScreenCaptureProvider::mWindowFilter(HWND   hwnd, LPARAM lParam) {
	CacheItemList *pCache = ((CacheItemList*)lParam);
	WINDOWPLACEMENT placement;
	GetWindowPlacement(hwnd, &placement);
	int isMinimized = placement.showCmd == SW_SHOWMINIMIZED;
	int isNormal = placement.showCmd == SW_SHOWMAXIMIZED;
	if (IsWindowVisible(hwnd) && isNormal) {
		ScreenCaptureProvider::getInstance()->addToCache(*pCache, hwnd);
	}
	return true;
}