#include "HID.h"

static HID hid;
static std::map<HANDLE, Device> map;

__declspec(dllexport) HANDLE	__stdcall HID_Open(UINT16 pid, UINT16 vid, UINT16 sn, UINT16 page, UINT16 usage);
__declspec(dllexport) bool		__stdcall HID_Write(HANDLE DeviceHandle, BYTE data[], int length);
__declspec(dllexport) bool		__stdcall HID_Read(HANDLE DeviceHandle, BYTE buffer[], int length);

HANDLE __stdcall HID_Open(UINT16 pid, UINT16 vid, UINT16 sn, UINT16 page, UINT16 usage)
{
	Device device{ 0 };
	bool found = false;
	hid.EnumDevice([pid, vid, sn, page, usage, &found](Device device) {
		if (device.Attributes.ProductID == pid &&
			device.Attributes.VendorID == vid &&
			device.Attributes.VersionNumber == sn &&
			device.Caps.UsagePage == page &&
			device.Caps.Usage == usage) {
			found = true;
			return true;
		}
		return false;
	});
	if (found) {
		map[device.DeviceHandle] = device;
		return device.DeviceHandle;
	}
	return 0;
}

bool __stdcall HID_Write(HANDLE DeviceHandle, BYTE data[], int length)
{
	if (map.find(DeviceHandle) == map.end())
		return false;
	return hid.Write(map[DeviceHandle], data, length);
}

bool __stdcall HID_Read(HANDLE DeviceHandle, BYTE buffer[], int length)
{
	
	if (map.find(DeviceHandle) == map.end())
		return false;
	return hid.Read(map[DeviceHandle], buffer, length);
}

BOOL APIENTRY DllMain( HMODULE Handle,
                       DWORD  Reason,
                       LPVOID
                     )
{
    if (Reason == DLL_PROCESS_DETACH)
    {
		for (auto& i : map) {
			CloseHandle(i.first);
		}
    }
    return TRUE;
}