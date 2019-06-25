// dllmain.cpp : 定义 DLL 应用程序的入口点。

#include "stdafx.h"
#include "HID.h"

#define DLLEXAM_EXPORTS
#ifdef DLLEXAM_EXPORTS
#define DLLAPI __declspec(dllexport)
#else
#define DLLAPI __declspec(dllimport)
#endif

HID hid;
std::map<HANDLE, Device> map;

DLLAPI HANDLE __stdcall OpenDevice(uint16_t pid, uint16_t vid, uint16_t sn, uint16_t page, uint16_t usage);
DLLAPI bool __stdcall WriteData(HANDLE handle, BYTE data[], int length);
DLLAPI bool __stdcall ReadData(HANDLE handle, BYTE buffer[], int length);

HANDLE __stdcall OpenDevice(uint16_t pid, uint16_t vid, uint16_t sn, uint16_t page, uint16_t usage)
{
	Device device;
	bool found = false;
	hid.enumDevices([pid, vid, sn, page, usage, &found](Device device) {
		if (device.attributes.ProductID == pid &&
			device.attributes.VendorID == vid &&
			device.attributes.VersionNumber == sn &&
			device.caps.UsagePage == page &&
			device.caps.Usage == usage) {
			found = true;
			return true;
		}
		return false;
	});
	if (found) {
		map[device.handle] = device;
		return device.handle;
	}
	return 0;
}

bool __stdcall WriteData(HANDLE handle, BYTE data[], int length)
{
	if (map.find[handle] == map.find.end())
		return false;
	return hid.writeData(map[handle], data, length);
}

bool __stdcall ReadData(HANDLE handle, BYTE buffer[], int length)
{
	if (map.find[handle] == map.find.end())
		return false;
	return hid.readData(map[handle], buffer, length);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
		for (auto i : map) {
			CloseHandle(i.first);
		}
        break;
    }
    return TRUE;
}