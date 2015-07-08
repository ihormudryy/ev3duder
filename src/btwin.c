/**
 * @file btwin.c
 * @author Ahmad Fatoum
 * @copyright (c) 2015 Ahmad Fatoum. Code available under terms of the GNU General Public License 2.0
 * @brief Windows bluetooth I/O using virtual COM port
 */
#include <stdlib.h>
#include <errno.h>

#include <windows.h>

#include "defs.h"

#define BT "COM1"
// ^ FIXME: add multiple COM ports (enumerate them)

/**
 * \param [in] virtual COM port or NULL
 * \return HANDLE Windows HANDLE to virtual COM port for use with bt_{read,write,close,error}
 * \brief opens COM Porte described by device. `NULL` leads to default action
 * \bug default value should be enumerating. Not hardcoded like in \p BT
 */ 
void *bt_open(const char *device)
{
	HANDLE handle = CreateFileA(device ?: BT, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	return handle != INVALID_HANDLE_VALUE ? handle : NULL;
}

/**
 * \param [in] handle handle returned by bt_open
 * \param [in] buf byte string to write, the first byte is omitted
 * \param [in] count number of characters to be written (including leading ignored byte)
 * \return status -1 on error. bytes read otherwise.	
 * \brief writes buf[1] till buf[count - 2] to device
 * \bug the first byte is omitted for compatiblity with the leading report byte demanded by \p hid_write. Wrapping HIDAPI could fix this.
 */ 
int bt_write(void* handle, const u8* buf, size_t count)
{
	DWORD dwBytesWritten;
	
	buf++;count--; // omit HID report number
	if (!WriteFile(handle, buf, count, &dwBytesWritten, NULL))
		return -1;
	
	return dwBytesWritten;
}

/**
 * \param [in] device handle returned by bt_open
 * \param [in] buf buffer to write to 
 * \param [in] count number of characters to be read
 * \param [in] milliseconds is ignored
 * \return status -1 on error. bytes read otherwise.	
 * \brief writes buf[1] till buf[count - 2] to device
 * \bug milliseconds is ignored
 */ 
int bt_read(void *handle, u8* buf, size_t count, int milliseconds)
{
	(void) milliseconds; // https://msdn.microsoft.com/en-us/library/windows/desktop/aa363190%28v=vs.85%29.aspx
	DWORD dwBytesRead;
	if (!ReadFile(handle, buf, count, &dwBytesRead, NULL))
		return -1;
	return dwBytesRead;
}
/**
 * \param [in] handle handle returned by bt_open
 * \brief calls CloseHandle on argument
 * \bug It takes whopping 3 seconds for the COM port to be reclaimable. If opened before that, the device is unusable for a minute.
 */ 
void bt_close(void *handle)
{
	CloseHandle(handle);
}

/**
 * \param [in] device handle returned by bt_open
 * \return message An error string
 * \brief Returns an error string describing the last error occured
 * \bug it's useless. Could be done with GetLastError + FormatMessageW
 */
const wchar_t *bt_error(void* fd_) { (void)fd_; return L"Errors not implemented yet";}

