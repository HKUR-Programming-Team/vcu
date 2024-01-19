#include <UtilsLib/Inc/CANManager.hpp>

namespace VehicleControlUnit::UtilsLib {

UtilsLib::ErrorState CANManager::init()
{
	mLogger.LogInfo("Initialization of " + mCanPortName + " Starts");
	// Initialize CAN
	auto error = HAL_FDCAN_Start(&mCanHandler);
	if (error != HAL_OK)
	{
		mLogger.LogError("Initialization of HAL_CAN failed in " + mCanPortName +  ". Please raise to developers");
		return ErrorState::INIT_FAIL;
	}

	// Activate interrupts
	if (mReceiveInterrupt == CANReceiveInterrupt::Interrupt0)
	{
		error = HAL_FDCAN_ActivateNotification(&mCanHandler, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, FDCAN_TX_BUFFER0);
	}
	else
	{
		error = HAL_FDCAN_ActivateNotification(&mCanHandler, FDCAN_IT_RX_FIFO1_NEW_MESSAGE, FDCAN_TX_BUFFER0);
	}

	if (error != HAL_OK)
	{
		mLogger.LogError("Activation of FDCAN_IT_RX_FIFO_NEW_MESSAGE in " + mCanPortName +  " failed. Please raise to developers");
		return ErrorState::INIT_FAIL;
	}

	mLogger.LogInfo("Initialization of " + mCanPortName +  " Success");
	return ErrorState::INIT_SUCCESS;
}

void CANManager::SetTransmitHeader(
			const uint32_t messageIdentifier,
			const bool identifierExtension,
			const CANFrameType frameType,
			const CANDataLength dataLength)
{
	mTransmitHeader.Identifier = messageIdentifier;

	if (identifierExtension)
	{
		mTransmitHeader.IdType = FDCAN_EXTENDED_ID;
	}
	else
	{
		mTransmitHeader.IdType = FDCAN_STANDARD_ID;
	}

	if (frameType == CANFrameType::DATA)
	{
		mTransmitHeader.TxFrameType = FDCAN_DATA_FRAME;
	}
	else
	{
		mTransmitHeader.TxFrameType = FDCAN_REMOTE_FRAME;
	}

	switch (dataLength)
	{
		case CANDataLength::ZERO: mTransmitHeader.DataLength = FDCAN_DLC_BYTES_0; break;
		case CANDataLength::ONE: mTransmitHeader.DataLength = FDCAN_DLC_BYTES_1; break;
		case CANDataLength::TWO: mTransmitHeader.DataLength = FDCAN_DLC_BYTES_2; break;
		case CANDataLength::THREE: mTransmitHeader.DataLength = FDCAN_DLC_BYTES_3; break;
		case CANDataLength::FOUR: mTransmitHeader.DataLength = FDCAN_DLC_BYTES_4; break;
		case CANDataLength::FIVE: mTransmitHeader.DataLength = FDCAN_DLC_BYTES_5; break;
		case CANDataLength::SIX: mTransmitHeader.DataLength = FDCAN_DLC_BYTES_6; break;
		case CANDataLength::SEVEN: mTransmitHeader.DataLength = FDCAN_DLC_BYTES_7; break;
		case CANDataLength::EIGHT: mTransmitHeader.DataLength = FDCAN_DLC_BYTES_8; break;
	}

	mTransmitHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	mTransmitHeader.BitRateSwitch = FDCAN_BRS_OFF;
	mTransmitHeader.FDFormat = FDCAN_CLASSIC_CAN;
	mTransmitHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	mTransmitHeader.MessageMarker = 0;
}

UtilsLib::ErrorState CANManager::SendMessage(const uint8_t message[8])
{
	mLogger.LogInfo("TODO: Send CAN message");

	// TODO :: use HAL_CAN_GetTxMailboxesFreeLevel() to check whether a mailbox is free
	// What to do if none is free? Implausible? abortPendingMessage?

	// Store the data to the buffer
	for (uint32_t i = 0; i < 8; i++)
	{
		mTransmitBuffer[i] = message[i];
	}

	// Then request transmission of a message using HAL_CAN_AddTxMessage()
	const auto returnStatus = HAL_FDCAN_AddMessageToTxFifoQ(&mCanHandler, &mTransmitHeader, mTransmitBuffer);
	mLogger.LogInfo("TODO:: Check TxFifo full or not by HAL_FDCAN_GetTxFifoFreeLevel");

	if (returnStatus == HAL_OK)
	{
		mLogger.LogInfo("TODO: Send CAN message HAL OK " + mCanPortName);
		return UtilsLib::ErrorState::CAN_MSG_TRANSMIT_SUCCESS;
	}
	else if (returnStatus == HAL_ERROR)
	{
		mLogger.LogInfo("TODO: Send CAN message HAL ERROR");
		return UtilsLib::ErrorState::CAN_MSG_TRANSMIT_FAIL;
	}

	// TODO :: Do appropriate action according to the return state: implausible? send again?
	return UtilsLib::ErrorState::CAN_MSG_TRANSMIT_FAIL;
}

UtilsLib::ErrorState CANManangerForBMSAndMCU::MessageReceiveHandler(const FDCAN_RxHeaderTypeDef& header, const uint8_t message[8])
{
	// TODO :: Map CAN message ID to action (call a function from another library)
	// You can call mBMSInterface.MessageReceiveHandler(header, message) to pass the message to BMS interface library
	mLogger.LogInfo("TODO: Map the received CAN Message's ID to an action for CANManangerForBMSAndMCU");
	return UtilsLib::ErrorState::CAN_MSG_RECEIVE_SUCCESS;
}

UtilsLib::ErrorState CANManangerForSensors::MessageReceiveHandler(const FDCAN_RxHeaderTypeDef& header, const uint8_t message[8])
{
	// TODO :: Map CAN message ID to action (call a function from another library)
	mLogger.LogInfo("TODO: Map the received CAN Message's ID to an action for CANManangerForSensors");
	return UtilsLib::ErrorState::INIT_SUCCESS;
}

}
