#pragma once
#include "stdafx.h"

#include <SetupAPI.h>
#include <hidsdi.h>
#include <hidpi.h>
#include <hidusage.h>

#pragma comment(lib,"hid.lib")
#pragma comment(lib,"setupapi.lib")

extern "C" void __stdcall      HidD_GetHidGuid(OUT LPGUID hidGuid);
extern "C" BOOLEAN __stdcall   HidD_GetAttributes(IN HANDLE device, OUT HIDD_ATTRIBUTES *attributes);
extern "C" BOOLEAN __stdcall   HidD_GetManufacturerString(IN HANDLE device, OUT void *buffer, IN ULONG bufferLen);
extern "C" BOOLEAN __stdcall   HidD_GetProductString(IN HANDLE device, OUT void *buffer, IN ULONG bufferLen);
extern "C" BOOLEAN __stdcall   HidD_GetSerialNumberString(IN HANDLE device, OUT void *buffer, IN ULONG bufferLen);
extern "C" BOOLEAN __stdcall   HidD_GetFeature(IN HANDLE device, OUT void *reportBuffer, IN ULONG bufferLen);
extern "C" BOOLEAN __stdcall   HidD_SetFeature(IN HANDLE device, IN void *reportBuffer, IN ULONG bufferLen);
extern "C" BOOLEAN __stdcall   HidD_GetNumInputBuffers(IN HANDLE device, OUT ULONG *numBuffers);
extern "C" BOOLEAN __stdcall   HidD_SetNumInputBuffers(IN HANDLE device, OUT ULONG numBuffers);

struct Device {
	HIDD_ATTRIBUTES attributes;
	HIDP_CAPS caps;
	BYTE reportIDFuture, reportIDOutput;
	HANDLE handle;
};

class HID
{
private:
	BYTE getReportID(const Device &device, HANDLE handle, HIDP_REPORT_TYPE type);

#ifdef UNICODE
	std::wstring getDevPath(HDEVINFO devinfo, SP_DEVICE_INTERFACE_DATA & devInfoData);
#else
	std::string getDevPath(HDEVINFO devinfo, SP_DEVICE_INTERFACE_DATA & devInfoData);
#endif
public:

	void enumDevices(std::function<bool(const Device&)> callback);

	bool writeData(const Device & device, BYTE data[], int length);
	bool readData(const Device & device, BYTE data[], int length);
};

