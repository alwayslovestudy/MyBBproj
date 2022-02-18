#include"MyBB.h"



NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath);

NTSTATUS DispatchIO(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
#pragma alloc_text(INIT, DriverEntry)




NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	NTSTATUS status = STATUS_SUCCESS;
	PDEVICE_OBJECT deviceObject = NULL;
	UNICODE_STRING deviceName;
	UNICODE_STRING symbol_Link_Name;
	RtlInitUnicodeString(&deviceName, BB_DEVICE_NAME);
	RtlInitUnicodeString(&symbol_Link_Name, BB_SYM_LNK_NAME);
	UNREFERENCED_PARAMETER(RegistryPath);
	//创建设备
	status = IoCreateDevice(DriverObject, 0, &deviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &deviceObject);
	if (!NT_SUCCESS(status))
	{
		DPRINT("Create Device Faield err：%x\n", status);
		return status;
	}
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchIO;

	status = IoCreateSymbolicLink(&symbol_Link_Name, &deviceName);
	if (!NT_SUCCESS(status))
	{
		DPRINT("Create Symlnkname Faield err：%x\n", status);
		IoDeleteDevice(&deviceObject);
		return status;
	}






















}
