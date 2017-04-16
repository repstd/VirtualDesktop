#pragma once
#include "VDDesktopLayoutStrategy.h"
class VDDefaultDesktopLayout:public VDDesktopLayoutStrategy{
public:
	VDDefaultDesktopLayout(){};
	~VDDefaultDesktopLayout(){};
	virtual LayoutParaList solve(const CacheItemList& windows);
};

