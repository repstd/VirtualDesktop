#pragma once
#include "osgViewer/Viewer"
#include "ScreenCaptureProvider.h"
#include "VDDesktopLayoutStrategy.h"
#include <vector>
class VDOSGViewer :public osgViewer::Viewer {
public:
	VDOSGViewer();
	VDOSGViewer(const VDOSGViewer& copy, const osg::CopyOp& op = osg::CopyOp::SHALLOW_COPY) :osgViewer::Viewer() {}
	META_Object(osg, VDOSGViewer)
	VDOSGViewer(int frame){};
	~VDOSGViewer(){}
	void setupViewer(osg::ref_ptr<osg::GraphicsContext::Traits> trait);
	void updateTextureList(CacheItemList& windows);
	virtual int run();
	void selectNode(osg::ref_ptr<osg::Node> node);
	void clickNode(osg::ref_ptr<osg::Node> node,int offsetx,int offsety);
	void moveCamare(float x,float y,float z,float w);
	osg::ref_ptr<osg::Group> getRootView();
private:
	osg::ref_ptr<osg::Node> getChild(const CacheItem& cacheWindowInfo,int index);
	osg::ref_ptr<osg::GraphicsContext::Traits> mTraits;
	std::unique_ptr<VDDesktopLayoutStrategy> mLayoutStrategy;
	osg::ref_ptr<osg::Group> mGroupNode;
	CacheItemList mCacheItemList;
	std::string mSelectedNode;
};
class PickHandler : public osgGA::GUIEventHandler {
public:
	PickHandler(osg::ref_ptr<VDOSGViewer> viewer);
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
	virtual void accept(osgGA::GUIEventHandlerVisitor& v);
private:
	bool handleNodePick(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, const osg::Matrixd& MVPW);
	bool handleKeyboard(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,const osg::Matrixd& MVPW);
	osg::ref_ptr<VDOSGViewer> mViewer;
};
