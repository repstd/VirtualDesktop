#include <memory>
#include "VDIMUManager.h"
#include "VDIMU.h"


VDIMUManager& VDIMUManager::getInstance() {
	static VDIMUManager inst;
	return inst;
}

void VDIMUManager::startReadingData() {
	this->mIMUDevice->connectToDevice();
	this->mIMUDevice->calibration();
	this->mIMUDevice->start();
}

void VDIMUManager::stopReadingData() {
	this->mIMUDevice->exit();
}

void VDIMUManager::registerObserver(std::shared_ptr<VDIMUObserver> observer) {
	this->mIMUDevice->registerObserver(observer);
}

void VDIMUManager::unRegisterObserver(std::shared_ptr<VDIMUObserver> observer) {
	this->mIMUDevice->unRegisterObserver(observer);
}

VDIMUManager::VDIMUManager(){
	this->mIMUDevice = std::make_shared<VDIMUDevice>();
}


VDIMUManager & VDIMUManager::operator=(const VDIMUManager & other) {
	// TODO: insert return statement here
	return *this;
}

VDIMUManager::~VDIMUManager() {
	this->mIMUDevice.reset();
}
