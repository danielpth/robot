/**
 * @file    xmodem.c
 * @author  Ferenc Nemeth
 * @date    21 Dec 2018
 * @brief   This module is the implementation of the Xmodem protocol.
 *
 *          Copyright (c) 2018 Ferenc Nemeth - https://github.com/ferenc-nemeth
 */

#include "xmodem.h"
#include <string.h>
#define UART_TIMEOUT 1000

/* Global variables. */
static uint8_t xmodem_packet_number = 1u; /**< Packet number counter. */
static uint32_t xmodem_actual_flash_address = 0u; /**< Address where we have to write. */
static uint8_t x_first_packet_received = false; /**< First packet or not. */

/* Local functions. */
static uint16_t xmodem_calc_crc(uint8_t *data, uint16_t length);
static xmodem_status xmodem_handle_packet(uint8_t size);
static xmodem_status xmodem_error_handler(uint8_t *error_number,
		uint8_t max_error_number);

char xmodem_trace[50] = {0};
/**
 * @brief   This function is the base of the Xmodem protocol.
 *          When we receive a header from UART, it decides what action it shall take.
 * @param   void
 * @return  void
 */
void xmodem_receive(void) {
	volatile xmodem_status status = X_OK;
	uint8_t error_number = 0u;
	uint8_t data;

	x_first_packet_received = false;
	xmodem_packet_number = 1u;
	xmodem_actual_flash_address = FLASH_APP_START_ADDRESS;

	/* Loop until there isn't any error (or until we jump to the user application). */
	while (X_OK == status) {
		uint8_t header = 0x00u;

		/* Get the header from UART. */
		HAL_StatusTypeDef comm_status = HAL_UART_Receive(&huart1, &header, 1u,
		UART_TIMEOUT);

		/* Spam the host (until we receive something) with ACSII "C", to notify it, we want to use CRC-16. */
		if ((HAL_OK != comm_status) && (false == x_first_packet_received)) {
			data = X_C;
			HAL_UART_Transmit(&huart1, &data, 1u, UART_TIMEOUT);
		}
		/* Uart timeout or any other errors. */
		else if ((HAL_OK != comm_status) && (true == x_first_packet_received)) {
			status = xmodem_error_handler(&error_number, X_MAX_ERRORS);
			strcat (xmodem_trace, "1 ");
		} else {
			/* Do nothing. */
		}

		/* The header can be: SOH, STX, EOT and CAN. */
		xmodem_status packet_status = X_ERROR;
		switch (header) {
		/* 128 or 1024 bytes of data. */
		case X_SOH:
		case X_STX:
			/* If the handling was successful, then send an ACK. */
			packet_status = xmodem_handle_packet(header);
			if (X_OK == packet_status) {
				data = X_ACK;
				HAL_UART_Transmit(&huart1, &data, 1u, UART_TIMEOUT);
			}
			/* If the error was flash related, then immediately set the error counter to max (graceful abort). */
			else if (X_ERROR_FLASH == packet_status) {
				error_number = X_MAX_ERRORS;
				status = xmodem_error_handler(&error_number, X_MAX_ERRORS);
				strcat (xmodem_trace, "2 ");

			}
			/* Error while processing the packet, either send a NAK or do graceful abort. */
			else {
				status = xmodem_error_handler(&error_number, X_MAX_ERRORS);
				strcat (xmodem_trace, "3 ");
				sprintf (&xmodem_trace[strlen(xmodem_trace)+1], "(%d)", packet_status);
			}
			break;
			/* End of Transmission. */
		case X_EOT:
			/* ACK, feedback to user (as a text), then jump to user application. */
			data = X_ACK;
			HAL_UART_Transmit(&huart1, &data, 1u, UART_TIMEOUT);
			HAL_UART_Transmit(&huart1, (uint8_t*) "\n\rFirmware updated!\n\r",
					21u,
					UART_TIMEOUT);
			HAL_UART_Transmit(&huart1,
					(uint8_t*) "Jumping to user application...\n\r", 32u,
					UART_TIMEOUT);
			flash_jump_to_app();
			break;
			/* Abort from host. */
		case X_CAN:
			status = X_ERROR;
			break;
		default:
			/* Wrong header. */
			if (HAL_OK == comm_status) {
				status = xmodem_error_handler(&error_number, X_MAX_ERRORS);
				strcat (xmodem_trace, "4 ");
			}
			break;
		}
	}
}

/**
 * @brief   Calculates the CRC-16 for the input package.
 * @param   *data:  Array of the data which we want to calculate.
 * @param   length: Size of the data, either 128 or 1024 bytes.
 * @return  status: The calculated CRC.
 */
static uint16_t xmodem_calc_crc(uint8_t *data, uint16_t length) {
	uint16_t crc = 0u;
	while (length) {
		length--;
		crc = crc ^ ((uint16_t) *data++ << 8u);
		for (uint8_t i = 0u; i < 8u; i++) {
			if (crc & 0x8000u) {
				crc = (crc << 1u) ^ 0x1021u;
			} else {
				crc = crc << 1u;
			}
		}
	}
	return crc;
}

/**
 * @brief   This function handles the data packet we get from the xmodem protocol.
 * @param   header: SOH or STX.
 * @return  status: Report about the packet.
 */
static xmodem_status xmodem_handle_packet(uint8_t header) {
	xmodem_status status = X_OK;
	uint16_t size = 0u;

	/* 2 bytes for packet number, 1024 for data, 2 for CRC*/
	uint8_t received_packet_number[X_PACKET_NUMBER_SIZE];
	uint8_t received_packet_data[X_PACKET_1024_SIZE];
	uint8_t received_packet_crc[X_PACKET_CRC_SIZE];

	/* Get the size of the data. */
	if (X_SOH == header) {
		size = X_PACKET_128_SIZE;
	} else if (X_STX == header) {
		size = X_PACKET_1024_SIZE;
	} else {
		/* Wrong header type. This shoudn't be possible... */
		status |= X_ERROR;
	}

	HAL_StatusTypeDef comm_status = HAL_OK;
	/* Get the packet number, data and CRC from UART. */
	comm_status |= HAL_UART_Receive(&huart1, received_packet_number,
	X_PACKET_NUMBER_SIZE, UART_TIMEOUT);
	comm_status |= HAL_UART_Receive(&huart1, received_packet_data, size,
	UART_TIMEOUT);
	comm_status |= HAL_UART_Receive(&huart1, received_packet_crc,
	X_PACKET_CRC_SIZE, UART_TIMEOUT);
	/* Merge the two bytes of CRC. */
	uint16_t crc_received =
			((uint16_t) received_packet_crc[X_PACKET_CRC_HIGH_INDEX] << 8u)
					| ((uint16_t) received_packet_crc[X_PACKET_CRC_LOW_INDEX]);
	/* We calculate it too. */
	uint16_t crc_calculated = xmodem_calc_crc(&received_packet_data[0u], size);

	/* Communication error. */
	if (HAL_OK != comm_status) {
		status |= X_ERROR_UART;
	}

	/* If it is the first packet, then erase the memory. */
	if ((X_OK == status) && (false == x_first_packet_received)) {
		if (FLASH_OK == flash_erase(FLASH_APP_START_ADDRESS)) {
			x_first_packet_received = true;
		} else {
			status |= X_ERROR_FLASH;
		}
	}

	/* Error handling and flashing. */
	if (X_OK == status) {
		if (xmodem_packet_number != received_packet_number[0u]) {
			/* Packet number counter mismatch. */
			status |= X_ERROR_NUMBER;
		}
		if (255u
				!= (received_packet_number[X_PACKET_NUMBER_INDEX]
						+ received_packet_number[X_PACKET_NUMBER_COMPLEMENT_INDEX])) {
			/* The sum of the packet number and packet number complement aren't 255. */
			/* The sum always has to be 255. */
			status |= X_ERROR_NUMBER;
		}
		if (crc_calculated != crc_received) {
			/* The calculated and received CRC are different. */
			status |= X_ERROR_CRC;
		}
	}

	/* Do the actual flashing (if there weren't any errors). */
	if ((X_OK == status)
			&& (FLASH_OK
					!= flash_write(xmodem_actual_flash_address,
							(uint32_t*) &received_packet_data[0u],
							(uint32_t) size / 4u))) {
		/* Flashing error. */
		status |= X_ERROR_FLASH;
	}

	/* Raise the packet number and the address counters (if there weren't any errors). */
	if (X_OK == status) {
		xmodem_packet_number++;
		xmodem_actual_flash_address += size;
	}

	return status;
}

/**
 * @brief   Handles the xmodem error.
 *          Raises the error counter, then if the number of the errors reached critical, do a graceful abort, otherwise send a NAK.
 * @param   *error_number:    Number of current errors (passed as a pointer).
 * @param   max_error_number: Maximal allowed number of errors.
 * @return  status: X_ERROR in case of too many errors, X_OK otherwise.
 */
static xmodem_status xmodem_error_handler(uint8_t *error_number,
		uint8_t max_error_number) {
	xmodem_status status = X_OK;
	uint8_t data;
	/* Raise the error counter. */
	(*error_number)++;
	/* If the counter reached the max value, then abort. */
	if ((*error_number) >= max_error_number) {
		/* Graceful abort. */
		data = X_CAN;
		HAL_UART_Transmit(&huart1, &data, 1u, UART_TIMEOUT);
		HAL_UART_Transmit(&huart1, &data, 1u, UART_TIMEOUT);
		status = X_ERROR;
	}
	/* Otherwise send a NAK for a repeat. */
	else {
		data = X_NAK;
		HAL_UART_Transmit(&huart1, &data, 1u, UART_TIMEOUT);
		status = X_OK;
	}
	return status;
}
