#pragma once
#include "VDIMU.h"
class VDesktopRenderer :public VDIMUObserver {
public:
	virtual void start() = 0;
	virtual void setup() = 0;
};
