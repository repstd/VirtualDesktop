#include "ScreenCaptureProvider.h"
#include "ScreenVideoCapture.h"
#include "VDesktopManager.h"
#include <vector>
#include <iostream>
#include <memory>
#include <unordered_set>
void testScreenCapture() {
	class VDCaptureCallback :public ScreenCaptureCallback {
	public:
		VDCaptureCallback() {

		}
		void onCapture(CacheItemList& cache){
			std::unordered_set<std::string> curSet;
			for (CacheItemList::iterator iter = cache.begin(); iter != cache.end(); iter++) {
				std::string title = (*iter)->mTitle;
				curSet.insert(title);
				cv::namedWindow(title, CV_WINDOW_AUTOSIZE);
				cv::imshow(title, (*iter)->mImage);
				cv::waitKey(1);
			}
			for (std::unordered_set<std::string>::iterator iter = mPreWindowSet.begin(); iter != mPreWindowSet.end(); iter++) {
				std::string title = *iter;
				if (curSet.find(title) == curSet.end()) {
					cv::destroyWindow(title);
				}
			}
			mPreWindowSet = curSet;
		}
	private:
		std::unordered_set<std::string> mPreWindowSet;
	};
	ScreenCaptureCallback* callback = new VDCaptureCallback();
	ScreenCaptureProvider::getInstance()->registerCallback(callback);
	ScreenCaptureProvider::getInstance()->startCapture(100);
	ScreenCaptureProvider::getInstance()->unregisterCallback();
}
int main() {
	VDesktopManager::getInstance().createDesktop(VDesktopManager::RenderType::OSG)->loop();
	//testScreenCapture();
	return 0;
}
