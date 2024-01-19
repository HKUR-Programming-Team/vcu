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
		mCanManagerForBMSAndMCU(mLogger, canHandlerForBMSAndMCU, "CANForBMSAndMCU", mBMSInterface, mMCUInterface, UtilsLib::CANReceiveInterrupt::Interrupt0),
		mCanManagerForSensors(mLogger, canHandlerForSensor, "CANForSensors", UtilsLib::CANReceiveInterrupt::Interrupt1),
		mSensorInterface(mLogger, mDataStore, mADCManager, Settings::throttleSignalADCIndex1, Settings::throttleSignalADCIndex2),
		mBMSInterface(mLogger, mDataStore),
		mMCUInterface(mLogger, mDataStore, mCanManagerForBMSAndMCU),
//		mMCUErrorManager(mLogger, mDataStore, Settings::implausibleThresholdInterval),
//		mReadyToDriveManager(mLogger, mDataStore, Settings::readyToDriveSoundDuration),
		mSetupFailed{false}
	{}

	void Setup();
	void Loop();

	void HAL_FDCAN_RxFifo0Callback(const FDCAN_RxHeaderTypeDef& header, const uint8_t message[8]);
	void HAL_FDCAN_RxFifo1Callback(const FDCAN_RxHeaderTypeDef& header, const uint8_t message[8]);

private:
	UtilsLib::Logger mLogger;
	DataStoreLib::DataStore mDataStore;

	UtilsLib::ADCManager mADCManager;
	UtilsLib::CANManangerForBMSAndMCU mCanManagerForBMSAndMCU;
	UtilsLib::CANManangerForSensors mCanManagerForSensors;

	SensorInterfaceLib::SensorInterface mSensorInterface;
	BMSInterfaceLib::BMSInterface mBMSInterface;
	MCUInterfaceLib::MCUInterface mMCUInterface;
//	MCUInterfaceLib::MCUErrorManager mMCUErrorManager;
//	ReadyToDriveLib::ReadyToDrive mReadyToDriveManager;

	bool mSetupFailed;
};

}


