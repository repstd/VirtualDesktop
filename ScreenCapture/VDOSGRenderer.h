#pragma once
#include <osgViewer/Viewer>
#include <osg/Node>
#include "VDesktopRenderer.h"
#include "VDOSGViewer.h"
class VDOSGRenderer : public VDesktopRenderer{
public:
	VDOSGRenderer(const osg::ref_ptr<VDOSGViewer>& Viewer);
	~VDOSGRenderer();
	virtual void start();
	virtual void setup();
protected:
	virtual void onDataChanged(const VDPara & para);
private:
	osg::ref_ptr<VDOSGViewer> mViewer;
};

