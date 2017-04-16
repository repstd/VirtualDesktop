#pragma once
#include <opencv2\opencv.hpp>
#include "ScreenVideoCapture.h"
#include <OpenThreads\Mutex>
#include <OpenThreads\Thread>
#include <vector>
class ScreenCaptureCallback;
class ScreenCaptureWorker;
struct WindowInfo;

typedef WindowInfo CacheItem;
typedef std::vector<std::shared_ptr<CacheItem>> CacheItemList;

class ScreenCaptureProvider {
public:
	friend class ScreenCaptureWorker;
	static ScreenCaptureProvider* getInstance();
	void registerCallback(ScreenCaptureCallback* callback);
	void unregisterCallback();
	void frame();
	void startCapture(float fps);
	std::vector<std::shared_ptr<ScreenCaptureWorker>>& startCaptureAsyncTask(float fps,int count);
	void startCaptureAsync(float fps);
private:
	ScreenCaptureProvider(ScreenCaptureCallback& ScreenCaptureCallback);
	ScreenCaptureProvider();
	~ScreenCaptureProvider();
	ScreenCaptureProvider& operator=(const ScreenCaptureCallback& other);
	void captureSync(bool lock);
	void addToCache(CacheItemList& cache, HWND hwnd);

	static BOOL CALLBACK mWindowFilter(HWND   hwnd, LPARAM lParam);

	std::shared_ptr<ScreenVideoCapture>		mCapture;
	std::shared_ptr<ScreenCaptureCallback>	mCallback;
	std::shared_ptr<OpenThreads::Mutex>		mMutex;
	float									mSleepTime;
};
class ScreenCaptureCallback {
public:
	ScreenCaptureCallback(){};
	ScreenCaptureCallback(const ScreenCaptureCallback*){};
	virtual void onCapture(CacheItemList& cache){};
};
class ScreenCaptureWorker:public OpenThreads::Thread { 
public:
	void run();
};
struct WindowInfo{
	cv::Mat		mImage;
	HWND		mHwnd;
	int			mX;
	int			mY;
	char		mTitle[200];
};
