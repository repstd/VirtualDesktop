#include "VDDesktopImpl.h"
#include "VDIMUManager.h"


VDDesktopImpl::VDDesktopImpl() {
}


VDDesktopImpl::~VDDesktopImpl() {
	VDIMUManager::getInstance().stopReadingData();
}

VDDesktopImpl::VDDesktopImpl(const std::shared_ptr<VDesktopRenderer>& renderer) {
	setup(renderer);
}
void VDDesktopImpl::setup(const std::shared_ptr<VDesktopRenderer>& renderer) {
	this->mRenderer = renderer;
	this->mRenderer->setup();
	VDIMUManager::getInstance().registerObserver(renderer);
}
void VDDesktopImpl::loop() {
	VDIMUManager::getInstance().startReadingData();
	this->mRenderer->start();
}
