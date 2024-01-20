#pragma once

#include <BMSInterfaceLib/Inc/BMSInterface.hpp>
#include <DataStoreLib/Inc/DataStore.hpp>
#include <UtilsLib/Inc/ADCManager.hpp>
#include <UtilsLib/Inc/Logger.hpp>
#include <UtilsLib/Inc/CANManager.hpp>
#include <MainLib/Inc/setttings.hpp>
//#include <MCUInterfaceLib/Inc/MCUErrorManager.hpp>
#include <MCUInterfaceLib/Inc/MCUInterface.hpp>
//#include <ReadyToDriveLib/Inc/ReadyToDrive.hpp>
#include <SensorInterfaceLib/Inc/SensorInterface.hpp>

#include <stm32h7xx.h>

namespace VehicleControlUnit::MainLib {

class Main
{

public:
	Main(FDCAN_HandleTypeDef& canHandlerForBMSAndMCU, FDCAN_HandleTypeDef& canHandlerForSensor, ADC_HandleTypeDef& adcHandler) :
		mLogger{Settings::spamLoggingEnabled,
				Settings::infoLoggingEnabled,
				Settings::errorLoggingEnabled,
				Settings::customLoggingEnabled},
		mDataStore(),
		mADCManager(mLogger, adcHandler, "ADC1", Settings::ADCDMABufferLength),
		mSensorInterface(mLogger, mDataStore, mADCManager, Settings::throttleSignalADCIndex1, Settings::throttleSignalADCIndex2),
		mCanManagerForBMSAndMCU(mLogger, canHandlerForBMSAndMCU, "CANForBMSAndMCU", mBMSInterface, mMCUInterface, UtilsLib::CANReceiveFIFONum::ZERO),
		mCanManagerForSensors(mLogger, canHandlerForSensor, "CANForSensors", mSensorInterface, UtilsLib::CANReceiveFIFONum::ONE),
		mBMSInterface(mLogger, mDataStore),
		mMCUInterface(mLogger, mDataStore, mCanManagerForBMSAndMCU),
//		mMCUErrorManager(mLogger, mDataStore, Settings::implausibleThresholdInterval),
//		mReadyToDriveManager(mLogger, mDataStore, Settings::readyToDriveSoundDuration),
		mSetupFailed{false}
	{}

	void Setup();
	void Loop();

private:
	UtilsLib::Logger mLogger;
	DataStoreLib::DataStore mDataStore;

	UtilsLib::ADCManager mADCManager;
	SensorInterfaceLib::SensorInterface mSensorInterface;
	UtilsLib::CANManangerForBMSAndMCU mCanManagerForBMSAndMCU;
	UtilsLib::CANManangerForSensors mCanManagerForSensors;

	BMSInterfaceLib::BMSInterface mBMSInterface;
	MCUInterfaceLib::MCUInterface mMCUInterface;
//	MCUInterfaceLib::MCUErrorManager mMCUErrorManager;
//	ReadyToDriveLib::ReadyToDrive mReadyToDriveManager;

	bool mSetupFailed;
};

}


