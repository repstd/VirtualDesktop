#pragma once
#include "VDesktop.h"
#include <memory>
class VDesktopManager {
public:
	static enum RenderType{
		OSG = 0,
		OGL = 1,
		DX = 2
	};
	static VDesktopManager& getInstance(){ static VDesktopManager inst; return inst; };
	std::shared_ptr<VDesktop> createDesktop(RenderType type);
private:
	VDesktopManager(){};
	VDesktopManager(const VDesktopManager& other){};
	VDesktopManager& operator=(const VDesktopManager& other){};
	~VDesktopManager(){};
};
