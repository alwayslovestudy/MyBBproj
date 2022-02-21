#include<ntddk.h>
#include"MyBB.h"



NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath);

NTSTATUS DispatchIO(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
void  DriverUnload(PDRIVER_OBJECT pDriverObject);
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
		KdPrint(("Create Device Faield err：%x\n", status));
		return status;
	}
	DriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchIO;
	DriverObject->MajorFunction[IRP_MJ_READ] = DispatchIO;
	DriverObject->MajorFunction[IRP_MJ_WRITE] = DispatchIO;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchIO;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchIO;
	DriverObject->DriverUnload = DriverUnload;

	status = IoCreateSymbolicLink(&symbol_Link_Name, &deviceName);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Create Symlnkname Faield err：%x\n", status));
		IoDeleteDevice(deviceObject);
		return status;
	}
	KdPrint(("kd....\n"));
	KdPrint(("create device success"));
	return status;


}


NTSTATUS DispatchIO(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	UNREFERENCED_PARAMETER(Irp);
	NTSTATUS status = STATUS_SUCCESS;
	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}
void  DriverUnload(PDRIVER_OBJECT pDriverObject)
{
	UNICODE_STRING symLnkName;
	RtlInitUnicodeString(&symLnkName, BB_SYM_LNK_NAME);
	IoDeleteSymbolicLink(&symLnkName);
	IoDeleteDevice(pDriverObject->DeviceObject);
	return;

}

