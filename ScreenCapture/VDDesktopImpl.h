#pragma once
#include "VDesktop.h"
#include "VDesktopRenderer.h"
class VDDesktopImpl :public VDesktop {
public:
	VDDesktopImpl(const std::shared_ptr<VDesktopRenderer>& renderer);
	VDDesktopImpl();
	~VDDesktopImpl();

	virtual void setup(const std::shared_ptr<VDesktopRenderer>& renderer);
	virtual void loop();

private:
	std::shared_ptr<VDesktopRenderer> mRenderer;
};

