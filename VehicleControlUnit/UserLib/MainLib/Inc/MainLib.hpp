#pragma once

#include <BMSInterfaceLib/Inc/BMSInterface.hpp>
#include <DataStoreLib/Inc/DataStore.hpp>
#include <UtilsLib/Inc/ADCManager.hpp>
#include <UtilsLib/Inc/Logger.hpp>
#include <UtilsLib/Inc/CANManager.hpp>
#include <MainLib/Inc/setttings.hpp>
#include <MCUInterfaceLib/Inc/MCUErrorManager.hpp>
#include <MCUInterfaceLib/Inc/MCUInterface.hpp>
#include <ReadyToDriveLib/Inc/ReadyToDrive.hpp>
#include <SensorInterfaceLib/Inc/SensorInterface.hpp>

#include <stm32f1xx.h>

namespace VehicleControlUnit::MainLib {

class Main
{

public:
	Main(CAN_HandleTypeDef& canHandler, ADC_HandleTypeDef& adcHandler) :
		mLogger{Settings::spamLoggingEnabled,
				Settings::infoLoggingEnabled,
				Settings::errorLoggingEnabled,
				Settings::customLoggingEnabled},
		mDataStore(),
		mADCManager(mLogger, adcHandler, "ADC1", Settings::ADCDMABufferLength),
		mSensorInterface(mLogger, mDataStore, mADCManager, Settings::throttleSignalADCIndex1, Settings::throttleSignalADCIndex2),
		mCANManager(mLogger, canHandler, "CAN1", mBMSInterface, mMCUInterface, mSensorInterface),
		mBMSInterface(mLogger, mDataStore),
		mMCUInterface(mLogger, mDataStore, mCANManager),
		mMCUErrorManager(mLogger, mDataStore, Settings::implausibleThresholdInterval),
		mReadyToDriveManager(mLogger, mDataStore, Settings::readyToDriveSoundDuration),
		mSetupFailed{false}
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

	bool mSetupFailed;
};

} // namespace VehicleControlUnit::MainLib


