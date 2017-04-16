#include "VDOSGRenderer.h"
#include <osgGA/TrackballManipulator>

VDOSGRenderer::VDOSGRenderer(const osg::ref_ptr<VDOSGViewer>& viewer) {
	this->mViewer = viewer;
}


VDOSGRenderer::~VDOSGRenderer() {
	this->mViewer.release();
}
void VDOSGRenderer::setup() {
	RECT resolution;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &resolution);
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->x = 0;
	traits->y = 0;
	traits->width = resolution.right - resolution.left;
	traits->height = resolution.bottom - resolution.top - 1;
	traits->windowDecoration = false;
	traits->doubleBuffer = true;
	traits->sharedContext = 0;
	traits->useCursor = true;
	mViewer->setThreadingModel(osgViewer::Viewer::ThreadingModel::SingleThreaded);
	mViewer->addEventHandler(new PickHandler(mViewer));
	mViewer->setupViewer(traits);
}
void VDOSGRenderer::onDataChanged(const VDPara& para) {
	mViewer->moveCamare(para.rx, para.ry, para.rz, para.rw);
}
void VDOSGRenderer::start() {
	this->mViewer->run();
}
