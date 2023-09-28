#include <UtilsLib/Inc/CANManager.hpp>

namespace VehicleControlUnit { namespace UtilsLib {

UtilsLib::ErrorState CANManager::init()
{
	logger.LogInfo("Initialization of CAN Manager Starts");
	// Initialize CAN
	auto error = HAL_CAN_Start(&canHandler);
	if (error == HAL_ERROR)
	{
		logger.LogError("Initialization of HAL_CAN failed. Please raise to developers");
		return ErrorState::INIT_FAIL;
	}

	// Activate interrupts
	error = HAL_CAN_ActivateNotification(&canHandler, CAN_IT_RX_FIFO0_MSG_PENDING);
	if (error == HAL_ERROR)
	{
		logger.LogError("Activation of CAN_IT_RX_FIFO0_MSG_PENDING failed. Please raise to developers");
		return ErrorState::INIT_FAIL;
	}

	logger.LogInfo("Initialization of CAN Manager Success");
	return ErrorState::INIT_SUCCESS;
}

void CANManager::SetTransmitHeader(
			const uint32_t messageIdentifier,
			const uint32_t messageLength,
			const bool identifierExtension,
			const bool isRemoteTransmissionRequest,
			const bool transmitGlobalTime)
{
	// StdId, ExtId, IDE
	if (identifierExtension)
	{
		transmitHeader.ExtId = messageIdentifier;
		transmitHeader.StdId = 0;
		transmitHeader.IDE = CAN_ID_EXT;
	}
	else
	{
		transmitHeader.ExtId = 0;
		transmitHeader.StdId = messageIdentifier;
		transmitHeader.IDE = CAN_ID_STD;
	}

	// DLC
	transmitHeader.DLC = messageLength;

	// RTR
	if (isRemoteTransmissionRequest)
	{
		transmitHeader.RTR = 1;
	}
	else
	{
		transmitHeader.RTR = 0;
	}

	// TransmitGlobalTime
	if (transmitGlobalTime)
	{
		transmitHeader.TransmitGlobalTime = FunctionalState::ENABLE;
	}
	else
	{
		transmitHeader.TransmitGlobalTime = FunctionalState::DISABLE;
	}

	logger.LogSpam("TransmitHeader is set for " + canPortName +
			". transmitHeader: {StdId: " + std::to_string(transmitHeader.StdId) +
			". ExtId: " + std::to_string(transmitHeader.ExtId) +
			". IDE: " + ((transmitHeader.ExtId == CAN_ID_STD) ? "CAN_ID_STD" : "CAN_ID_EXT") +
			". DLC: " + std::to_string(transmitHeader.DLC) +
			". RTR: " + std::to_string(transmitHeader.RTR) +
			". transmitGlobalTime: " + std::to_string(transmitGlobalTime) + "}");
}

UtilsLib::ErrorState CANManager::SendMessage(const uint8_t message[8])
{
	logger.LogInfo("TODO: Send CAN message");

	// TODO :: use HAL_CAN_GetTxMailboxesFreeLevel() to check whether a mailbox is free
	// What to do if none is free? Implausible? abortPendingMessage?

	// Store the data to the buffer
	for (uint32_t i = 0; i < transmitHeader.DLC; i++)
	{
		transmitBuffer[i] = message[i];
	}

	// Then request transmission of a message using HAL_CAN_AddTxMessage()
	const auto returnStatus = HAL_CAN_AddTxMessage(&canHandler, &transmitHeader, transmitBuffer, &lastMailboxUsed);

	if (returnStatus == HAL_OK)
	{
		logger.LogInfo("TODO: Send CAN message HAL OK");
	}
	else if (returnStatus == HAL_ERROR)
	{
		logger.LogInfo("TODO: Send CAN message HAL ERROR");
	}
	// TODO :: Do appropriate action according to the return state: implausible? send again?
	return UtilsLib::ErrorState::INIT_SUCCESS;
}

// TODO :: CAN Transmit
// TODO :: How to map the received CAN message's ID to an action?

}}
