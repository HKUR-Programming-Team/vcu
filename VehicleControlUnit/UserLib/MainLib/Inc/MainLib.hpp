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
#include <MainLib/Inc/Config.hpp>

#include <stm32f1xx.h>

#include <optional>
#include <functional>

namespace VehicleControlUnit::MainLib {

class Main
{

public:
	Main(const MainLib::Config::Config& config, CAN_HandleTypeDef& canHandler, ADC_HandleTypeDef& adcHandler, std::function<void()> displayFatalError) :
		mConfig{config},
		mLogger{mConfig.mLoggerConfig.spamLoggingEnabled,
				mConfig.mLoggerConfig.infoLoggingEnabled,
				mConfig.mLoggerConfig.errorLoggingEnabled,
				mConfig.mLoggerConfig.customLoggingEnabled},
		mDataStore(),
		mADCManager(mLogger, 
				adcHandler, 
				"ADC1", 
				config.mAdcConfig.ADCDMABufferLength),
		mSensorInterface(mLogger, mDataStore, mADCManager, mConfig),
		mCANManager(mLogger, canHandler, "CAN1", mBMSInterface, mMCUInterface, mSensorInterface),
		mBMSInterface(mLogger, mDataStore),
		mMCUInterface(mLogger, mDataStore, mCANManager, mConfig),
		mMCUErrorManager(mLogger, mDataStore, mConfig.mErrorConfig.implausibleThresholdInterval),
		mReadyToDriveManager(mLogger, mDataStore, mConfig),
		mDashboardInterface(mLogger, mDataStore, mConfig),
		mSetupFailed{false},
		DisplayFatalError{displayFatalError}
	{}

	void Setup();
	void Loop();

private:
	const Config::Config mConfig;
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


