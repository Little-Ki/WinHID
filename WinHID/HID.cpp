#include "HID.h"

const static GUID HID_GUID = { 0x4D1E55B2, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30} };

BYTE HID::ReportID(const Device& device, HANDLE device_handle, HIDP_REPORT_TYPE type)
{
	UINT16					CapsSize;
	PHIDP_PREPARSED_DATA	DataPtr;
	BYTE					Out;

	if (device.Caps.NumberOutputButtonCaps != 0) {
		CapsSize = (type == HIDP_REPORT_TYPE::HidP_Feature) ?
			device.Caps.NumberFeatureButtonCaps :
			device.Caps.NumberOutputButtonCaps;
		auto ButtonCaps = std::make_unique<HIDP_BUTTON_CAPS>(CapsSize);
		HidD_GetPreparsedData(device_handle, &DataPtr);
		HidP_GetButtonCaps(type, ButtonCaps.get(), &CapsSize, DataPtr);
		Out = ButtonCaps.get()[0].ReportID;
	}
	else {
		CapsSize = (type == HIDP_REPORT_TYPE::HidP_Feature) ?
			device.Caps.NumberFeatureValueCaps :
			device.Caps.NumberOutputValueCaps;
		auto ValueCaps = std::make_unique<HIDP_VALUE_CAPS>(CapsSize);
		HidD_GetPreparsedData(device_handle, &DataPtr);
		HidP_GetValueCaps(type, ValueCaps.get(), &CapsSize, DataPtr);
		Out = ValueCaps.get()[0].ReportID;
	}

	return Out;
}

STRING HID::DevicePath(HDEVINFO device_info_handle, SP_DEVICE_INTERFACE_DATA& device_info_data)
{
	DWORD Size = 0;

	if (!SetupDiGetDeviceInterfaceDetail(
		device_info_handle,
		&device_info_data,
		NULL,
		0,
		&Size,
		NULL)) {
		SP_DEVICE_INTERFACE_DETAIL_DATA Detail;

		Detail.cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

		if (SetupDiGetDeviceInterfaceDetail(
			device_info_handle,
			&device_info_data,
			&Detail,
			Size,
			&Size,
			NULL)) {
			return STRING(Detail.DevicePath);
		}
	}

	return STRING();
}

void HID::EnumDevice(std::function<bool(const Device&)> callback)
{
	uint32_t index = 0;
	HDEVINFO DeviceInfoHandle = SetupDiGetClassDevs(
		&HID_GUID,
		NULL,
		0,
		DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
	);
	HIDD_ATTRIBUTES Attributes;

	SP_DEVICE_INTERFACE_DATA DeviceInfoData;
	DeviceInfoData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

	while (SetupDiEnumDeviceInterfaces(DeviceInfoHandle, NULL, &HID_GUID, index, &DeviceInfoData)) {
		std::wstring path = DevicePath(DeviceInfoHandle, DeviceInfoData);
		uint32_t flag = 0;
		HANDLE devHandle =
			CreateFile(
				path.c_str(),
				GENERIC_WRITE | GENERIC_READ,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				OPEN_EXISTING,
				flag,
				NULL
			);
		if (devHandle) {
			Attributes.Size = sizeof(_HIDD_ATTRIBUTES);
			if (HidD_GetAttributes(devHandle, &Attributes)) {
				Device _Device;
				HIDP_CAPS Caps;
				PHIDP_PREPARSED_DATA pData;
				HidD_GetPreparsedData(devHandle, &pData);
				HidP_GetCaps(pData, &Caps);
				HidD_FreePreparsedData(pData);
				_Device.ReportIDFuture = ReportID(_Device, devHandle, HIDP_REPORT_TYPE::HidP_Feature);
				_Device.ReportIDOutput = ReportID(_Device, devHandle, HIDP_REPORT_TYPE::HidP_Output);
				_Device.DeviceHandle = devHandle;
				if (callback(_Device)) {
					SetupDiDestroyDeviceInfoList(DeviceInfoHandle);
					return;
				}
				CloseHandle(devHandle);
			}

		}
		index++;
	};
	SetupDiDestroyDeviceInfoList(DeviceInfoHandle);
}

bool HID::Write(const Device& device, BYTE* data, int length)
{
	if (device.Caps.OutputReportByteLength == 0 ||
		length != device.Caps.OutputReportByteLength - 1) {
		return false;
	}
	else {
		DWORD Count;

		auto Temp = std::make_unique<BYTE>(
			device.Caps.OutputReportByteLength
			);

		Temp.get()[0] = device.ReportIDOutput;

		for (int i = 1; i < device.Caps.OutputReportByteLength; i++) {
			Temp.get()[i] = data[i - 1];
		}

		bool Out = WriteFile(
			device.DeviceHandle,
			Temp.get(),
			device.Caps.OutputReportByteLength,
			&Count,
			0
		);

		return Out;
	}
}

bool HID::Read(const Device& device, BYTE* data, int length)
{
	if (device.Caps.InputReportByteLength == 0 ||
		length != device.Caps.InputReportByteLength) {
		return false;
	}
	else {

		HANDLE EventHandle = CreateEvent(NULL, true, false, NULL);

		OVERLAPPED Overlap = { 0,0,0,0,EventHandle };

		bool Out = ReadFile(
			device.DeviceHandle,
			data,
			length,
			0,
			&Overlap
		);

		WaitForSingleObject(EventHandle, 500);

		return Out;
	}
}

