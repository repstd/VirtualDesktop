#include "VDOSGViewer.h"
#include "ScreenCaptureProvider.h"
#include "Logger.h"
#include <string>
#include <osg/TexMat>
#include <osg/Texture2D>
#include <osg/ImageStream>
#include <assert.h>
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osg/MatrixTransform>
#include <osg/TextureRectangle>
#include <algorithm>
#include <string>
class VDOSGCaptureCallback :public ScreenCaptureCallback {
public:
	VDOSGCaptureCallback(osg::ref_ptr<VDOSGViewer>& viewer) {
		mViewer = viewer;
	}
	void onCapture(CacheItemList& cache) {
		mViewer->updateTextureList(cache);
	}
private:
	osg::ref_ptr<VDOSGViewer> mViewer;
};
VDOSGViewer::VDOSGViewer() :osgViewer::Viewer() {
	ScreenCaptureProvider::getInstance()->registerCallback(new VDOSGCaptureCallback(osg::ref_ptr<VDOSGViewer>(this)));
}
void VDOSGViewer::setupViewer(osg::ref_ptr<osg::GraphicsContext::Traits> traits) {
	mTraits = traits;
	mGroupNode = new osg::Group();
	osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());
	getCamera()->setViewport(0, 0, traits->width, traits->height);
	getCamera()->setGraphicsContext(gc.get());
	setSceneData(mGroupNode);
}
bool cmp(const std::shared_ptr<CacheItem>& lhs, const std::shared_ptr<CacheItem>& rhs) {
	return lhs->mImage.cols < rhs->mImage.cols ||
		(lhs->mImage.cols == rhs->mImage.cols&&lhs->mImage.rows < rhs->mImage.rows);
}
void VDOSGViewer::updateTextureList(CacheItemList& data) {
	std::sort(data.begin(), data.end(), cmp);
	std::shared_ptr<CacheItem> selected = nullptr;
	int numChildren = this->mGroupNode->getNumChildren();
	this->mGroupNode->removeChildren(0, numChildren);
	this->mCacheItemList.clear();
	if (this->mSelectedNode.c_str() != nullptr) {
		for (CacheItemList::iterator iter = data.begin(); iter != data.end(); iter++) {
			CacheItem item = *(iter->get());
			if (strcmp(item.mTitle, this->mSelectedNode.c_str()) == 0) {
				selected = *iter;
				data.erase(iter);
				break;
			}
		}
	}
	if (selected != nullptr) {
		data.insert(data.begin(), selected);
	}
	int index = 0;
	for (CacheItemList::iterator iter = data.begin(); iter != data.end(); iter++) {
		CacheItem item = *(iter->get());
		osg::ref_ptr<osg::Node> node = getChild(item, index);
		if (node == nullptr || node.get() == nullptr)
			continue;
		index++;
		this->mGroupNode->addChild(node);
		this->mCacheItemList.push_back(*iter);
	}
}
osg::ref_ptr<osg::Node> VDOSGViewer::getChild(const CacheItem& cacheWindowInfo, int index) {
	osg::ref_ptr<osg::Image> image = new osg::Image();
	int width = cacheWindowInfo.mImage.cols;
	int height = cacheWindowInfo.mImage.rows;
	int channel = cacheWindowInfo.mImage.channels();
	if (width < 300 || height < 300 || (strstr(cacheWindowInfo.mTitle, "cmd") != NULL))
		return nullptr;
	image->allocateImage(width, height, channel, GL_BGRA, GL_UNSIGNED_BYTE);
	BYTE* dst = image->data();
	BYTE* src = cacheWindowInfo.mImage.data;
	unsigned int rowStepInBytes = width * 4;
	for (int j = 0; j < height; j++) {
		memcpy(dst + (height - 1 - j)*rowStepInBytes, src + j*rowStepInBytes, rowStepInBytes);
	}
	osg::ref_ptr<osg::Geometry> pictureQuad = osg::createTexturedQuadGeometry(osg::Vec3(0.f, 0.f, 0.f),
		osg::Vec3(image->s(), 0.f, 0.f),
		osg::Vec3(0.f, 0.f, image->t()),
		0.f, 0.f, image->s(), image->t());
	osg::ref_ptr<osg::TextureRectangle> textureRect = new osg::TextureRectangle(image);
	textureRect->setTextureWidth(width);
	textureRect->setTextureHeight(height);
	textureRect->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
	textureRect->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	textureRect->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
	textureRect->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
	pictureQuad->getOrCreateStateSet()->setTextureAttributeAndModes(0, textureRect.get(), osg::StateAttribute::ON);
	pictureQuad->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	geode->setDataVariance(osg::Object::DYNAMIC);
	geode->addDrawable(pictureQuad.get());
	osg::StateSet *state = geode->getOrCreateStateSet();
	state->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF);
	osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform;

	float scale = 0.8f;
	int initialOffsetX = this->mTraits->width / 2 + this->mTraits->width / 2 - (width*scale / 2);
	int initialiOffsetY = this->mTraits->height / 2 + this->mTraits->height / 2 - (height*scale / 2);

	trans->setMatrix(osg::Matrix::scale(scale, 1.0f, scale) *
		osg::Matrix::rotate(0.0f, 0.0f, 0.0f, 1.0f)*
		osg::Matrix::translate(initialOffsetX + index*this->mTraits->width, 0, initialiOffsetY));
	trans->addChild(geode);
	trans->setName(cacheWindowInfo.mTitle);
	return osg::ref_ptr<osg::Node>(trans.release());
}
void VDOSGViewer::selectNode(osg::ref_ptr<osg::Node> node) {
	if (node == NULL || node.get() == nullptr) {
		this->mSelectedNode.clear();
		return;
	}
	this->mSelectedNode = node->getName();
}
BOOL CALLBACK clickChildFIlter(HWND   hwnd, LPARAM lParam) {
	RECT client;
	GetClientRect(hwnd, &client);
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);
	x -= client.left;
	y -= client.top;
	printf("click child window: %d %d %d %d\n", x, y, client.left, client.top);
	LPARAM newPara= MAKELPARAM(x, y);
	PostMessage(hwnd, WM_LBUTTONDOWN, 1, newPara);
	PostMessage(hwnd, WM_LBUTTONUP, 0, newPara);
	InvalidateRect(hwnd, NULL, TRUE);
	UpdateWindow(hwnd);
	return true;
}
void clickWindow(HWND handle,int offsetx,int offsety) {
	LPARAM lParam = MAKELPARAM(offsetx, offsety);
	PostMessage(handle, WM_LBUTTONDOWN, 1, lParam);
	PostMessage(handle, WM_LBUTTONUP, 0, lParam);
	InvalidateRect(handle, NULL, TRUE);
	UpdateWindow(handle);
	printf("click window: %d %d\n", offsetx, offsety);
	//EnumChildWindows(handle, clickChildFIlter, lParam);
}
void VDOSGViewer::clickNode(osg::ref_ptr<osg::Node> node, int offsetx, int offsety) {
	if (node == NULL || node.get() == nullptr) {
		return;
	}
	std::string name = node->getName();
	if (name.c_str() == nullptr)
		return;
	for (CacheItemList::iterator iter = this->mCacheItemList.begin(); iter != this->mCacheItemList.end(); iter++) {
		std::shared_ptr<CacheItem> ptr = *iter;
		if (strcmp(name.c_str(), ptr->mTitle) == 0) {
			HWND handle = ptr->mHwnd;
			clickWindow(handle, offsetx, offsety);
			break;
		}
	}
}
void rotateCamera(const osg::ref_ptr<osg::Camera> camera, double angle) {
	osg::Vec3d eye, at, up;
	camera->getViewMatrixAsLookAt(eye, at, up);
	osg::Vec3d direction = at - eye;
	osg::Quat rotation(osg::DegreesToRadians(angle), osg::Z_AXIS, osg::DegreesToRadians(0.0), osg::Y_AXIS, osg::DegreesToRadians(0.0), osg::X_AXIS);
	osg::Matrixd rotationMatrix(rotation);
	direction = rotationMatrix * direction;
	at = eye + direction;
	camera->setViewMatrixAsLookAt(eye, at, up);
}

void moveCamera(const osg::ref_ptr<osg::Camera> camera, const osg::Vec3d& offset) {
	osg::Vec3d eye, at, up;
	camera->getViewMatrixAsLookAt(eye, at, up);
	eye += offset;
	at += offset;
	camera->setViewMatrixAsLookAt(eye, at, up);
}

float pre_x = INT_MIN, pre_y = INT_MIN, pre_z = INT_MIN, pre_w = INT_MIN;
void VDOSGViewer::moveCamare(float x, float y, float z, float w) {
	//printf("data %f %f %f %f\n", x, y, z, w);
	//TODO: respond to IMU data change
	if (pre_x != INT_MIN) {
		osg::ref_ptr<osgGA::GUIEventAdapter> e = getEventQueue()->createEvent();
		e->setEventType(osgGA::GUIEventAdapter::EventType::KEYUP);
		if (z - pre_z > 0) {
			//e->setKey(osgGA::GUIEventAdapter::KEY_Left);
			//e->setKey(osgGA::GUIEventAdapter::KEY_Left);
		}
		else if (z - pre_z < 0) {
			//e->setKey(osgGA::GUIEventAdapter::KEY_Right);
			//e->setKey(osgGA::GUIEventAdapter::KEY_Right);
		}
		getEventQueue()->addEvent(e);
	}
	pre_x = x;
	pre_y = y;
	pre_z = z;
	pre_w = w;
}

int VDOSGViewer::run() {
	if (!isRealized()) {
		realize();
	}
	osg::ref_ptr<osg::Camera> camera = getCamera();
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	camera->setProjectionMatrix(osg::Matrix::ortho2D(0.f, this->mTraits->width, 0, this->mTraits->height));
	float x = this->mTraits->width / 2, y = this->mTraits->height / 2;
	camera->setViewMatrixAsLookAt(
		osg::Vec3f(x, -1000.f, y), // eye
		osg::Vec3f(x, 0.f, y), // center
		osg::Vec3f(0.f, 0.f, 1.f)); // up vector
	while (!done()) {
		ScreenCaptureProvider::getInstance()->frame();
		frame();
		updateTraversal();
	}
	return 0;
}
osg::ref_ptr<osg::Group> VDOSGViewer::getRootView() {
	return mGroupNode;
}
PickHandler::PickHandler(osg::ref_ptr<VDOSGViewer> viewer) {
	this->mViewer = viewer;
}
void printMatrix(std::string desc, const osg::Matrixd& m) {
	std::cout << desc << std::endl;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			std::cout << m(i, j) << " ";
		}
		std::cout << std::endl;
	}
}
bool PickHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
	osg::ref_ptr<osg::Group> root = mViewer->getRootView();
	osg::Matrixd proj = mViewer->getCamera()->getProjectionMatrix();
	osg::Matrix MVPW = mViewer->getCamera()->getViewMatrix() * proj;
	MVPW.postMult(mViewer->getCamera()->getViewport()->computeWindowMatrix());
	switch (ea.getEventType()) {
	case osgGA::GUIEventAdapter::EventType::DOUBLECLICK:
	case osgGA::GUIEventAdapter::EventType::RELEASE:
		return handleNodePick(ea, aa, MVPW);
	case osgGA::GUIEventAdapter::EventType::KEYDOWN:
	case osgGA::GUIEventAdapter::EventType::KEYUP:
		return handleKeyboard(ea, aa, MVPW);
	}
	return false;
}
bool PickHandler::handleNodePick(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, const osg::Matrixd& MVPW) {
	osg::ref_ptr<osg::Group> root = mViewer->getRootView();
	bool isSelect = ea.getEventType() == osgGA::GUIEventAdapter::EventType::DOUBLECLICK;
	bool isRelease = ea.getEventType() == osgGA::GUIEventAdapter::EventType::RELEASE;
	if (isSelect) {
		mViewer->selectNode(nullptr);
	}
	int numChildren = root->getNumChildren();
	float x = ea.getX();
	float y = ea.getY();
	for (int i = 0; i < numChildren; i++) {
		osg::ref_ptr<osg::MatrixTransform> child = dynamic_cast<osg::MatrixTransform*>(root->getChild(i));
		if (child == nullptr)
			continue;
		osg::Matrix matrix = child->getMatrix();
		matrix.postMult(MVPW);

		osg::ref_ptr<osg::Geode> geode = dynamic_cast<osg::Geode*>(child->getChild(0));
		osg::BoundingBox bx = geode->getBoundingBox();
		osg::Vec3d center = bx.center()*matrix;
		osg::Vec3d scale = matrix.getScale();
		float width = (bx.xMax() - bx.xMin())*scale[0];
		float height = (bx.zMax() - bx.zMin())*scale[1];
		if (x >= (center[0] - width / 2) &&
			x <= (center[0] + width / 2) &&
			y >= (center[1] - height / 2) &&
			y <= (center[1] + height / 2)) {
			if (isSelect) {
				mViewer->selectNode(child);
			}
			else {
				int offsetx = (x - (center[0] - width / 2)) / scale[0];
				int offsety = (height - (y - (center[1] - height / 2))) / scale[1];
				mViewer->clickNode(child, offsetx, offsety);
			}
			break;
		}
	}
	return false;
}

bool PickHandler::handleKeyboard(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, const osg::Matrixd& MVPW) {
	if (ea.getEventType() == osgGA::GUIEventAdapter::EventType::KEYUP||ea.getEventType()==osgGA::GUIEventAdapter::EventType::KEYDOWN) {
		osg::ref_ptr<osg::Camera> camera = mViewer->getCamera();
		osg::ref_ptr<osg::GraphicsContext> gc = camera->getGraphicsContext();
		int hs = gc->getTraits()->width;
		int vs = gc->getTraits()->height;
		int step = 10;
		if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Left) {
			moveCamera(camera, osg::Vec3d(-1 * hs / (float)step, 0.f, 0.f));
		}
		else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Right) {
			moveCamera(camera, osg::Vec3d(hs / (float)step, 0.f, 0.f));
		}
		else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Up) {
			moveCamera(camera, osg::Vec3d(0.0f, 0.0f, vs / (float)step));
		}
		else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Down) {
			moveCamera(camera, osg::Vec3d(0.0f, 0.0f, -1 * vs / (float)step));
		}
	}
	return false;
}

void PickHandler::accept(osgGA::GUIEventHandlerVisitor& v) {
	v.visit(*this);
}