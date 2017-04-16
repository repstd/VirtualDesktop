#pragma once
#include <memory>
#include "VDesktopRenderer.h"
class VDesktop {
public:
	virtual void setup(const std::shared_ptr<VDesktopRenderer>& renderer) = 0;
	virtual void loop() = 0;
};

