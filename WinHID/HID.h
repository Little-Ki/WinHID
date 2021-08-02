#pragma once
#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容

#include <Windows.h>

#include <SetupAPI.h>
#include <hidsdi.h>
#include <hidpi.h>
#include <hidusage.h>

#include <iostream>
#include <vector>
#include <iomanip>

#include <map>
#include <memory>
#include <functional>

#pragma comment(lib,"hid.lib")
#pragma comment(lib,"setupapi.lib")

extern "C" void		__stdcall   HidD_GetHidGuid(OUT LPGUID hidGuid);
extern "C" BOOLEAN	__stdcall   HidD_GetAttributes(IN HANDLE device, OUT HIDD_ATTRIBUTES * attributes);
extern "C" BOOLEAN	__stdcall   HidD_GetManufacturerString(IN HANDLE device, OUT void* buffer, IN ULONG bufferLen);
extern "C" BOOLEAN	__stdcall   HidD_GetProductString(IN HANDLE device, OUT void* buffer, IN ULONG bufferLen);
extern "C" BOOLEAN	__stdcall   HidD_GetSerialNumberString(IN HANDLE device, OUT void* buffer, IN ULONG bufferLen);
extern "C" BOOLEAN	__stdcall   HidD_GetFeature(IN HANDLE device, OUT void* reportBuffer, IN ULONG bufferLen);
extern "C" BOOLEAN	__stdcall   HidD_SetFeature(IN HANDLE device, IN void* reportBuffer, IN ULONG bufferLen);
extern "C" BOOLEAN	__stdcall   HidD_GetNumInputBuffers(IN HANDLE device, OUT ULONG * numBuffers);
extern "C" BOOLEAN	__stdcall   HidD_SetNumInputBuffers(IN HANDLE device, OUT ULONG numBuffers);

struct Device {
	HIDD_ATTRIBUTES Attributes;
	HIDP_CAPS		Caps;
	BYTE			ReportIDFuture;
	BYTE			ReportIDOutput;
	HANDLE			DeviceHandle;
};

#ifdef UNICODE
using STRING = std::wstring;
#else
using STRING = std::string;
#endif

class HID
{
private:
	BYTE ReportID(const Device&, HANDLE, HIDP_REPORT_TYPE);

	STRING DevicePath(HDEVINFO, SP_DEVICE_INTERFACE_DATA&);
public:

	void EnumDevice(std::function<bool(const Device&)> callback);

	bool Write(const Device& device, BYTE* data, int length);
	bool Read(const Device& device, BYTE* data, int length);
};