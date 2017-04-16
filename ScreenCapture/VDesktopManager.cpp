#include "VDesktopManager.h"
#include "VDDesktopImpl.h"
#include "VDOSGRenderer.h"
#include "VDOSGViewer.h"
std::shared_ptr<VDesktop> VDesktopManager::createDesktop(RenderType type) {
	std::shared_ptr<VDesktopRenderer> render;
	switch (type) {
	case OSG:
	//TODO: add OpenGL/DirectX baded renderer implementation
	case OGL:
	case DX:
	default:
		render = std::make_shared<VDOSGRenderer>(osg::ref_ptr<VDOSGViewer>(new VDOSGViewer()));
		break;
	}
	return std::make_shared<VDDesktopImpl>(render);
}
