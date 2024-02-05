#pragma once

namespace VehicleControlUnit::UtilsLib {

enum class ErrorState
{
	INIT_SUCCESS,
	INIT_FAIL,
	CAN_MSG_TRANSMIT_SUCCESS,
	CAN_MSG_TRANSMIT_MAILBOX_FULL,
	ADC_DMA_INDEX_OUT_OF_RANGE,
	ADC_DMA_SUCCESS
};

}
