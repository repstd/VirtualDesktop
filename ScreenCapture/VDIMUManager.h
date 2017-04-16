#include <memory>
#include "./VDesktopRenderer.h"
class VDIMUManager {
public:
	static VDIMUManager& getInstance();
	void startReadingData();
	void stopReadingData();
	void registerObserver(std::shared_ptr<VDIMUObserver> observer);
	void unRegisterObserver(std::shared_ptr<VDIMUObserver> observer);
private:
	VDIMUManager();
	VDIMUManager(const VDIMUManager& other);
	VDIMUManager& operator=(const VDIMUManager& other);
	~VDIMUManager();
private:
	std::shared_ptr<VDIMUDevice> mIMUDevice;
};

