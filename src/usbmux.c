/**
* Copyright (c) 2021-2022 Analog Devices, Inc. All Rights Reserved. This
* software is proprietary and confidential to Analog Devices, Inc. and its
* licensors.
*/

#include "usbmux.h"

#ifndef _WIN32
#warning USB MUX only supported on Windows
#endif

/**
    * Open a handle to the USB MUX named pipe.
    * @param[out] phPipe Pointer to the USB MUX named pipe handle
    * @param dwTimeout Named pipe connection timeout
    * @return return code to indicate success or failure
    */
DWORD usbmux_open(HANDLE* phPipe, DWORD dwTimeout)
{
#ifdef _WIN32
    while (true)
    {
        *phPipe = CreateFile(
            USB_MUX_PIPE_NAME,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            0,
            NULL
        );

        // Break if we have a valid pipe handle
        if (*phPipe != INVALID_HANDLE_VALUE)
            break;

        if (GetLastError() != ERROR_PIPE_BUSY)
        {
            return (DWORD) USB_MUX_FAIL;
        }

        if (!WaitNamedPipe(USB_MUX_PIPE_NAME, dwTimeout))
        {
            return (DWORD) USB_MUX_TIMEOUT;
        }
    }

    // Connection successful, set pipe mode
    DWORD dwMode = PIPE_READMODE_MESSAGE;
    BOOL success = SetNamedPipeHandleState(
        *phPipe,
        &dwMode,
        NULL,
        NULL
    );

    if (!success)
    {
        return (DWORD) USB_MUX_FAIL;
    }
#endif
    return (DWORD) USB_MUX_OK;
}

/**
    * Close the handle to the USB MUX named pipe.
    * @param hPipe Handle to the USB MUX named pipe
    */
void usbmux_close(HANDLE hPipe)
{
#ifdef _WIN32
    CloseHandle(hPipe);
#endif
}

/**
    * Populate the given header with the appropriate values in the header format.
    * @param[out] pHeader The header buffer to populate
    * @param eType The message type
    * @param numBytes The number of bytes to be sent in the message (after the header)
    * @param endpoint The USB endpoint to communicate with
    * @param timeout The USB read / write timeout
    */
void usbmux_create_header(uint8_t* pHeader, UsbMuxMessageType eType, uint32_t numBytes, uint32_t endpoint, uint32_t timeout)
{
#ifdef _WIN32
    if (pHeader == NULL)
        return;

    pHeader[0] = (uint8_t) eType;
    for (int i=0; i<4; i++)
    {
        pHeader[i+1] = (uint8_t) ((numBytes >> i*8) & 0xFF);
        pHeader[i+5] = (uint8_t) ((endpoint >> i*8) & 0xFF);
        pHeader[i+9] = (uint8_t) ((timeout >> i*8) & 0xFF);
    }
#endif
}

/**
    * Read data from the given USB endpoint (blocking).
    * @param hPipe Handle to the USB MUX named pipe
    * @param[out] pBuffer Pointer to the buffer to receive the data
    * @param numBytes Number of bytes to read
    * @param endpoint USB endpoint to read from
    * @param timeout Read timeout (milliseconds)
    * @return USB_MUX_ERROR code to indicate success or failure
    */
USB_MUX_ERROR usbmux_read(HANDLE hPipe, uint8_t* pBuffer, uint32_t numBytes, uint32_t endpoint, uint32_t timeout)
{
#ifdef _WIN32
    if (numBytes > USB_MUX_BUFFER_SIZE)
        return USB_MUX_BUFFER_OVERRUN;

    BOOL ret = FALSE;
    DWORD bytesActual = 0;
    uint8_t pHeader[USB_MUX_MSG_HEADER_SIZE];

    usbmux_create_header(pHeader, READ, numBytes, endpoint, timeout);

    // Send the read message header
    ret = WriteFile(
        hPipe,
        pHeader,
        USB_MUX_MSG_HEADER_SIZE,
        &bytesActual,
        NULL
    );

    if (!ret || bytesActual != USB_MUX_MSG_HEADER_SIZE)
    {
        return USB_MUX_FAIL;
    }

    // Read the response from the server
    DWORD response;
    ret = ReadFile(
        hPipe,
        &response,
        USB_MUX_RESPONSE_SIZE,
        &bytesActual,
        NULL
    );

    if (!ret || bytesActual != USB_MUX_RESPONSE_SIZE || !response)
    {
        return USB_MUX_FAIL;
    }

    // Read back the data
    ret = ReadFile(
        hPipe,
        pBuffer,
        numBytes,
        &bytesActual,
        NULL
    );

    if (!ret || bytesActual != numBytes)
    {
        return USB_MUX_FAIL;
    }
#endif
    return USB_MUX_OK;
}

/**
    * Write data to the given USB endpoint (blocking).
    * @param pData Pointer to the USB MUX interface data
    * @param pBuffer Pointer to the buffer of data to write
    * @param numBytes Number of bytes to write
    * @param endpoint USB endpoint to write to
    * @param timeout Write timeout (milliseconds)
    * @return USB_MUX_ERROR code to indicate success or failure
    */
USB_MUX_ERROR usbmux_write(HANDLE hPipe, uint8_t* pBuffer, uint32_t numBytes, uint32_t endpoint, uint32_t timeout)
{
#ifdef _WIN32
    if (numBytes > USB_MUX_BUFFER_SIZE)
        return USB_MUX_BUFFER_OVERRUN;

    BOOL ret = FALSE;
    DWORD bytesActual = 0;
    uint8_t pHeader[USB_MUX_MSG_HEADER_SIZE];

    usbmux_create_header(pHeader, WRITE, numBytes, endpoint, timeout);

    // Send the write message header
    ret = WriteFile(
        hPipe,
        pHeader,
        USB_MUX_MSG_HEADER_SIZE,
        &bytesActual,
        NULL
    );

    if (!ret || bytesActual != USB_MUX_MSG_HEADER_SIZE)
    {
        return USB_MUX_FAIL;
    }

    ret = WriteFile(
        hPipe,
        pBuffer,
        numBytes,
        &bytesActual,
        NULL
    );

    if (!ret || bytesActual != numBytes)
    {
        return USB_MUX_FAIL;
    }

    // Read the response from the server
    DWORD response;
    ret = ReadFile(
        hPipe,
        &response,
        USB_MUX_RESPONSE_SIZE,
        &bytesActual,
        NULL
    );

    if (!ret || bytesActual != USB_MUX_RESPONSE_SIZE || !response)
    {
        return USB_MUX_FAIL;
    }
#endif
    return USB_MUX_OK;
}

/**
    * Lock the USB MUX connection.
    */
USB_MUX_ERROR usbmux_lock(HANDLE hPipe)
{
#ifdef _WIN32
	BOOL ret = FALSE;
	DWORD response = 0;
	DWORD bytesActual = 0;
	uint8_t pHeader[USB_MUX_MSG_HEADER_SIZE];

	usbmux_create_header(pHeader, LOCK, 0, 0, 0);

	ret = WriteFile(
		hPipe,
		pHeader,
		USB_MUX_MSG_HEADER_SIZE,
		&bytesActual,
		NULL
	);

	if (!ret || bytesActual != USB_MUX_MSG_HEADER_SIZE)
	{
		return USB_MUX_FAIL;
	}

	// Read the response from the server
	ret = ReadFile(
		hPipe,
		&response,
		USB_MUX_RESPONSE_SIZE,
		&bytesActual,
		NULL
	);

	if (!ret || bytesActual != USB_MUX_RESPONSE_SIZE)
	{
		return USB_MUX_FAIL;
	}

	if (!response)
	{
		// return busy if already locked
		return USB_MUX_BUSY;
	}
#endif
	return USB_MUX_OK;
}

/**
    * Unlock the USB MUX connection.
    */
USB_MUX_ERROR usbmux_unlock(HANDLE hPipe)
{
#ifdef _WIN32
    BOOL ret = FALSE;
    DWORD bytesActual = 0;
    uint8_t pHeader[USB_MUX_MSG_HEADER_SIZE];

    usbmux_create_header(pHeader, UNLOCK, 0, 0, 0);

    // Send the unlock message header
    ret = WriteFile(
        hPipe,
        pHeader,
        USB_MUX_MSG_HEADER_SIZE,
        &bytesActual,
        NULL
    );

    if (!ret || bytesActual != USB_MUX_MSG_HEADER_SIZE)
    {
        return USB_MUX_FAIL;
    }

    // Read the response from the server
    DWORD response;
    ret = ReadFile(
        hPipe,
        &response,
        USB_MUX_RESPONSE_SIZE,
        &bytesActual,
        NULL
    );

    if (!ret || bytesActual != USB_MUX_RESPONSE_SIZE || !response)
    {
        return USB_MUX_FAIL;
    }
#endif
    return USB_MUX_OK;
}

/**
    * Get the product ID for the connected USB device
    * @param hPipe Handle to the USB MUX named pipe
    * @param[out] pProductId Pointer to the product ID
    * @return USB_MUX_ERROR code to indicate success or failure
    */
USB_MUX_ERROR usbmux_get_product_id(HANDLE hPipe, uint16_t* pProductId)
{
#ifdef _WIN32
    BOOL ret = FALSE;
    DWORD bytesActual = 0;
    uint8_t pHeader[USB_MUX_MSG_HEADER_SIZE];

    usbmux_create_header(pHeader, GET_PID, 0, 0, 0);

    // Send the 'GET_PID' message header
    ret = WriteFile(
        hPipe,
        pHeader,
        USB_MUX_MSG_HEADER_SIZE,
        &bytesActual,
        NULL
    );

    if (!ret || bytesActual != USB_MUX_MSG_HEADER_SIZE)
    {
        return USB_MUX_FAIL;
    }

    // Read the response from the server
    DWORD response;
    ret = ReadFile(
        hPipe,
        &response,
        USB_MUX_RESPONSE_SIZE,
        &bytesActual,
        NULL
    );

    if (!ret || bytesActual != USB_MUX_RESPONSE_SIZE || !response)
    {
        return USB_MUX_FAIL;
    }

    *pProductId = (uint16_t) response;
#endif
    return USB_MUX_OK;
}

