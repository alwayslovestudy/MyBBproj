//#include<ntddk.h>
#include<ntifs.h>
#include"MyBB.h"



NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath);
NTSTATUS DispatchIO(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS TraverseVAD(PEPROCESS pEProcess);
NTSTATUS EnumProcessHandleTable(ULONG procPid);
void  DriverUnload(PDRIVER_OBJECT pDriverObject);
VOID CreateProcessNotify(IN HANDLE ParentId, IN HANDLE ProcessId, IN BOOLEAN Create);
NTSTATUS EnumProcessHandleTable(ULONG Pid);
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
		KdPrint(("Create Device Faildd err：%x\n", status));
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
		KdPrint(("Create Symlnkname Failed err：%x\n", status));
		IoDeleteDevice(deviceObject);
		return status;
	}
	/*HANDLE hPid = (HANDLE)1012;
	PEPROCESS pProcess;
	status = PsLookupProcessByProcessId(hPid, &pProcess);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("get EPROCESS failed\n"));
		return status;
	}

	TraverseVAD(pProcess);
	KdPrint(("Driver Entry Exit\n"));
	if (pProcess)
		ObDereferenceObject(pProcess);*/
		//枚举进程句柄表
	ULONG Pid = 1108;
	EnumProcessHandleTable(Pid);
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
		KdPrint(("mem :%llx - %llx type:%x\n", pRoot->StartingVpn, pRoot->EndingVpn, pRoot->u.VadFlags.VadType));
		TraverseTree(pRoot->RightChild);
		return status;
	}
}

NTSTATUS TraverseVAD(PEPROCESS pEProcess)
{
	PMM_AVL_TABLE pNode = (PMM_AVL_TABLE)((((INT64)pEProcess)) + VAD_OFFSET);

	PMMADDRESS_NODE pRoot = pNode->BalancedRoot.LeftChild;
	KdPrint(("vadroot:%llx\n", pRoot));
	TraverseTree((PMMVAD)pRoot);

}


typedef NTKERNELAPI
BOOLEAN(*_ExEnumHandleTable)(
	IN PHANDLE_TABLE,
	IN EX_ENUMERATE_HANDLE_ROUTINE,
	IN PVOID,
	OUT PHANDLE
	);


BOOLEAN MBHandleCallback(
	IN PHANDLE_TABLE_ENTRY HandleTableEntry,
	IN HANDLE Handle,
	IN PVOID EnumParameter
)
{
	KdPrint(("Handle:%x\n", Handle));
	KdPrint(("HandleTableEntry:%llx\n", HandleTableEntry));
	if (Handle == 0x38)
		return TRUE;


}

NTSTATUS EnumProcessHandleTable(ULONG Pid)
{
	PEPROCESS pProcess;
	NTSTATUS status = STATUS_SUCCESS;
	status = PsLookupProcessByProcessId((HANDLE)Pid, &pProcess);
	if (NT_SUCCESS(status))
	{
		UNICODE_STRING uFuncName;
		RtlInitUnicodeString(&uFuncName, L"ExEnumHandleTable");
		_ExEnumHandleTable ExEnumHandleTable = MmGetSystemRoutineAddress(&uFuncName);
		if (ExEnumHandleTable)
		{
			KdPrint(("get ExEnumHandleTable success\n"));
			PHANDLE_TABLE handleTable = *(PHANDLE_TABLE*)(PUCHAR)pProcess + 0x200;
			KdPrint(("handleTable:%llx", handleTable));
			LONG para = 0;
			BOOLEAN ret = ExEnumHandleTable(handleTable, &MBHandleCallback, &para, NULL);
			KdPrint(("ret:%x\n", ret));
			return status;
		}
	}
	status = STATUS_INVALID_ADDRESS;
	return status;








}










