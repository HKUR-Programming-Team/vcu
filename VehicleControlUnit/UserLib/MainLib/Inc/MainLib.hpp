#pragma once

#include <BMSInterfaceLib/Inc/BMSInterface.hpp>
#include <DataStoreLib/Inc/DataStore.hpp>
#include <DashboardInterfaceLib/Inc/DashboardInterface.hpp>
#include <UtilsLib/Inc/ADCManager.hpp>
#include <UtilsLib/Inc/Logger.hpp>
#include <UtilsLib/Inc/GPIOManager.hpp>
#include <UtilsLib/Inc/CANManager.hpp>
#include <MCUInterfaceLib/Inc/MCUErrorManager.hpp>
#include <MCUInterfaceLib/Inc/MCUInterface.hpp>
#include <ReadyToDriveLib/Inc/ReadyToDrive.hpp>
#include <SensorInterfaceLib/Inc/SensorInterface.hpp>
#include <MainLib/Inc/settings.hpp>

#include <stm32f1xx.h>

#include <optional>
#include <functional>

namespace VehicleControlUnit::MainLib {

class Main
{

public:
	Main(CAN_HandleTypeDef& canHandler, ADC_HandleTypeDef& adcHandler, std::function<void()> displayFatalError) :
		mLogger{Settings::spamLoggingEnabled,
				Settings::infoLoggingEnabled,
				Settings::errorLoggingEnabled,
				Settings::customLoggingEnabled},
		mDataStore(),
		mADCManager(mLogger, adcHandler, "ADC1", Settings::ADCDMABufferLength),
		mSensorInterface(mLogger, mDataStore, mADCManager, Settings::sensorInterfaceParameters),
		mCANManager(mLogger, canHandler, "CAN1", mBMSInterface, mMCUInterface, mSensorInterface),
		mBMSInterface(mLogger, mDataStore),
		mMCUInterface(mLogger, mDataStore, mCANManager, Settings::mcuInterfaceParameters),
		mMCUErrorManager(mLogger, mDataStore, Settings::implausibleThresholdInterval),
		mReadyToDriveManager(mLogger, mDataStore, Settings::readyToDriveParameters),
		mDashboardInterface(mLogger, mDataStore, Settings::dashboardInterfaceParameters),
		mSetupFailed{false},
		DisplayFatalError{displayFatalError}
	{}

	void Setup();
	void Loop();

private:
	UtilsLib::Logger mLogger;
	DataStoreLib::DataStore mDataStore;

	UtilsLib::ADCManager mADCManager;
	SensorInterfaceLib::SensorInterface mSensorInterface;
	UtilsLib::CANManager mCANManager;
	BMSInterfaceLib::BMSInterface mBMSInterface;
	MCUInterfaceLib::MCUInterface mMCUInterface;
	MCUInterfaceLib::MCUErrorManager mMCUErrorManager;
	ReadyToDriveLib::ReadyToDrive mReadyToDriveManager;
	DashboardInterfaceLib::DashboardInterface mDashboardInterface;

	bool mSetupFailed;
	std::function<void()> DisplayFatalError;
};

} // namespace VehicleControlUnit::MainLib


