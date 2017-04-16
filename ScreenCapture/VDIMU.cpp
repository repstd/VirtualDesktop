#include <OpenThreads\Atomic>
#include "VDIMU.h"
#include <iostream>
OpenThreads::Atomic VDIMUDevice::mIsStarted(0);

VDIMUDevice::VDIMUDevice() :OpenThreads::Thread() {

}

VDIMUDevice::~VDIMUDevice() {
}

void VDIMUDevice::connectToDevice() {
	if (mIsStarted == 0) {
		std::cout << "connect" << std::endl;
		imu_connectToDevice();
		mIsStarted++;
	}
}

void VDIMUDevice::exit() {
	if (mIsStarted > 0) {
		mIsStarted--;
	}
}

void VDIMUDevice::calibration() {
	imu_calibration();
}

float* VDIMUDevice::getRawData() {
	if (mIsStarted > 0) {
		return imu_readRawData();
	}
	else {
		return nullptr;
	}
}

void VDIMUDevice::run() {
	while (true) {
		if (mIsStarted == 0) {
			imu_exit();
			break;
		}
		float* data = getRawData();
		if (data != nullptr && sizeof(data) == 4) {
			notifyDataChange(VDPara(data));
		}
	}
}

void VDIMUObservable::registerObserver(std::shared_ptr<VDIMUObserver> observer) {
	this->mObservers.push_back(observer);
}

void VDIMUObservable::unRegisterObserver(std::shared_ptr<VDIMUObserver> observer) {
	std::vector<std::shared_ptr<VDIMUObserver>>::iterator iter = std::find(mObservers.begin(), mObservers.end(), observer);
	if (iter != mObservers.end()) {
		mObservers.erase(iter);
	}
}

void VDIMUObservable::notifyDataChange(const VDPara& data) {
	for (std::vector<std::shared_ptr<VDIMUObserver>>::iterator iter = mObservers.begin(); iter != mObservers.end(); iter++) {
		(*(iter))->onDataChanged(data);
	}
}
