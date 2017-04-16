#pragma once
#include <assert.h>
#include <vector>
#include <memory>
#include <algorithm>
#include <OpenThreads\Mutex>
#include <OpenThreads\Thread>
#include <OpenThreads\Atomic>
#include "PencilIMU.h"
typedef struct Para {
	float rx;
	float ry;
	float rz;
	float rw;
	Para(const Para& other) {
		rx = other.rx;
		ry = other.ry;
		rz = other.rz;
		rw = other.rw;
	}
	Para(float* input) {
		rx = input[0];
		ry = input[1];
		rz = input[2];
		rw = input[3];
	}

	static void reset(const std::shared_ptr<Para>& buff, float* input) {
		buff->rx = input[0];
		buff->ry = input[1];
		buff->rz = input[2];
		buff->rw = input[3];
	}
	static Para& fromRawData(float* raw) {
		Para data(raw);
		return data;
	}
} VDPara;

class VDIMUObserver {
protected:
	friend class VDIMUObservable;

	virtual void onDataChanged(const VDPara& para) = 0;
};

class VDIMUObservable {
public:
	VDIMUObservable() {}
	~VDIMUObservable() {}

	void registerObserver(std::shared_ptr<VDIMUObserver> observer);
	void unRegisterObserver(std::shared_ptr<VDIMUObserver> observer);
	void notifyDataChange(const VDPara& data);
private:
	std::vector<std::shared_ptr<VDIMUObserver>> mObservers;
};

class VDIMUDevice :public OpenThreads::Thread, public VDIMUObservable {
public:
	VDIMUDevice();
	~VDIMUDevice();
	void connectToDevice();
	void exit();
	void calibration();
	float* getRawData();
	void run();
private:
	static OpenThreads::Atomic mIsStarted;
};



