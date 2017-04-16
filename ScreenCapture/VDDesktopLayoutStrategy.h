#pragma once
#include <vector>
#include "./ScreenCaptureProvider.h"
typedef struct {
	double x;
	double y;
	double z;
	double rx;
	double ry;
	double rz;
} LayoutPara;
typedef std::vector<LayoutPara> LayoutParaList;
class VDDesktopLayoutStrategy {
public:
	VDDesktopLayoutStrategy(){};
	~VDDesktopLayoutStrategy(){};
	virtual LayoutParaList solve(const CacheItemList& windows) = 0;
};