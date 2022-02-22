#include<ntddk.h>
#include"MyBB.h"



NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath);
NTSTATUS DispatchIO(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
void  DriverUnload(PDRIVER_OBJECT pDriverObject);
VOID CreateProcessNotify(IN HANDLE ParentId, IN HANDLE ProcessId, IN BOOLEAN Create);
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
	KdPrint(("Driver Entry Exit\n"));
	return status;


}


NTSTATUS DispatchIO(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION  pIrpStack;
	pIrpStack = IoGetCurrentIrpStackLocation(Irp);
	PVOID ioBuffer = NULL;
	ioBuffer = Irp->AssociatedIrp.SystemBuffer;
	Irp->IoStatus.Information = 0;
	switch (pIrpStack->MajorFunction)
	{
	case IRP_MJ_DEVICE_CONTROL:  
	{
		ULONG ioControlCode;
		ioControlCode = pIrpStack->Parameters.DeviceIoControl.IoControlCode;
		switch (ioControlCode)
		{
		case IOCTL_PROC_CREATE_CALLBACK:
			status = PsSetCreateProcessNotifyRoutine(CreateProcessNotify, FALSE);
			if (!NT_SUCCESS(status))
				KdPrint(("reg CreateProcess Callback failed err:%x\n", status));

		}
		




	}



	}






	Irp->IoStatus.Status = status;
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
VOID CreateProcessNotify(IN HANDLE ParentId, IN HANDLE ProcessId, IN BOOLEAN Create)
{
	UNREFERENCED_PARAMETER(ParentId);
	if (Create)
		KdPrint((" PID: %x Create\n", ProcessId));
	else
		KdPrint((" PID: %x Terminate\n", ProcessId));


}








