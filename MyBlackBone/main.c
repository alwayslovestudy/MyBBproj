//#include<ntddk.h>
#include<ntifs.h>
#include"MyBB.h"



NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath);
NTSTATUS DispatchIO(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS TraverseVAD(PEPROCESS pEProcess);
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
	HANDLE hPid = (HANDLE)1464;
	PEPROCESS pProcess;
	status = PsLookupProcessByProcessId(hPid, &pProcess);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("get EPROCESS failed\n"));
		return status;
	}

	TraverseVAD(pProcess);


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
//进程创建或删除回调函数
VOID CreateProcessNotify(IN HANDLE ParentId, IN HANDLE ProcessId, IN BOOLEAN Create)
{
	UNREFERENCED_PARAMETER(ParentId);
	if (Create)
		KdPrint((" PID: %x Create\n", ProcessId));
	else
		KdPrint((" PID: %x Terminate\n", ProcessId));


}
//遍历二叉树
NTSTATUS TraverseTree(PMMVAD pRoot)
{

	NTSTATUS status = STATUS_SUCCESS;
	if (!pRoot)
	{
		status = STATUS_INVALID_ADDRESS;
		return status;
	}
	else
	{   //中序遍历
		TraverseTree(pRoot->LeftChild);
		KdPrint(("mem :%x -%x  type:%x\n", pRoot->StartingVpn, pRoot->EndingVpn, pRoot->u.VadFlags.VadType));
		TraverseTree(pRoot->RightChild);
		return status;
	}

}


NTSTATUS TraverseVAD(PEPROCESS pEProcess)
{
	PMM_AVL_TABLE pNode = (PMM_AVL_TABLE)(PCHAR)pEProcess + VAD_OFFSET;
	PMMADDRESS_NODE pRoot = pNode->BalancedRoot.RightChild;
	KdPrint(("vadroot:%llx\n", pRoot));
	//TraverseTree((PMMVAD)pRoot);


}



