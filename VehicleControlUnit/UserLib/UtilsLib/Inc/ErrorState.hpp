#pragma once

namespace VehicleControlUnit { namespace UtilsLib {

enum class ErrorState
{
	INIT_SUCCESS,
	INIT_FAIL,
	CAN_MSG_TRANSMIT_SUCCESS,
	CAN_MSG_TRANSMIT_FAIL,
	CAN_MSG_RECEIVE_FAIL,
	CAN_MSG_RECEIVE_SUCCESS
};

}}
