#pragma once
#ifndef  _MY_BB_HEADER
#define _MY_BB_HEADER

#define BB_DEVICE_NAME L"\\Device\\myBBDevice"
#define BB_SYM_LNK_NAME L"\\??\\symLnkBBDevice"
#define IOCTL_PROC_CREATE_CALLBACK (ULONG) CTL_CODE(FILE_DEVICE_UNKNOWN,0x800,METHOD_BUFFERED,FILE_READ_ACCESS|FILE_WRITE_ACCESS)



//VAD 相关类型 for win7 

#define VAD_OFFSET 0x440


typedef struct _MMADDRESS_NODE          // 5 elements, 0x28 bytes (sizeof) 
{
	union                               // 2 elements, 0x8 bytes (sizeof)  
	{
		/*0x000*/         INT64        Balance : 2;       // 0 BitPosition                   
		/*0x000*/         struct _MMADDRESS_NODE* Parent;
	}u1;
	/*0x008*/     struct _MMADDRESS_NODE* LeftChild;
	/*0x010*/     struct _MMADDRESS_NODE* RightChild;
	/*0x018*/     UINT64       StartingVpn;
	/*0x020*/     UINT64       EndingVpn;
}MMADDRESS_NODE, * PMMADDRESS_NODE;

typedef struct _MM_AVL_TABLE                          // 6 elements, 0x40 bytes (sizeof) 
{
	/*0x000*/     struct _MMADDRESS_NODE BalancedRoot;              // 5 elements, 0x28 bytes (sizeof) 
	struct                                            // 3 elements, 0x8 bytes (sizeof)  
	{
		/*0x028*/         UINT64       DepthOfTree : 5;                 // 0 BitPosition                   
		/*0x028*/         UINT64       Unused : 3;                      // 5 BitPosition                   
		/*0x028*/         UINT64       NumberGenericTableElements : 56; // 8 BitPosition                   
	};
	/*0x030*/     VOID* NodeHint;
	/*0x038*/     VOID* NodeFreeHint;
}MM_AVL_TABLE, * PMM_AVL_TABLE;


union _EX_PUSH_LOCK // Size=8
{
	struct
	{
		unsigned __int64 Locked : 1; // Size=8 Offset=0 BitOffset=0 BitCount=1
		unsigned __int64 Waiting : 1; // Size=8 Offset=0 BitOffset=1 BitCount=1
		unsigned __int64 Waking : 1; // Size=8 Offset=0 BitOffset=2 BitCount=1
		unsigned __int64 MultipleShared : 1; // Size=8 Offset=0 BitOffset=3 BitCount=1
		unsigned __int64 Shared : 60; // Size=8 Offset=0 BitOffset=4 BitCount=60
	};
	unsigned __int64 Value; // Size=8 Offset=0
	void* Ptr; // Size=8 Offset=0
};



typedef struct _MMVAD_FLAGS         // 7 elements, 0x8 bytes (sizeof) 
{
	/*0x000*/     UINT64       CommitCharge : 51; // 0 BitPosition                  
	/*0x000*/     UINT64       NoChange : 1;      // 51 BitPosition                 
	/*0x000*/     UINT64       VadType : 3;       // 52 BitPosition                 
	/*0x000*/     UINT64       MemCommit : 1;     // 55 BitPosition                 
	/*0x000*/     UINT64       Protection : 5;    // 56 BitPosition                 
	/*0x000*/     UINT64       Spare : 2;         // 61 BitPosition                 
	/*0x000*/     UINT64       PrivateMemory : 1; // 63 BitPosition                 
}MMVAD_FLAGS, * PMMVAD_FLAGS;

typedef struct _MMVAD_FLAGS2          // 9 elements, 0x4 bytes (sizeof) 
{
	/*0x000*/     UINT32       FileOffset : 24;     // 0 BitPosition                  
	/*0x000*/     UINT32       SecNoChange : 1;     // 24 BitPosition                 
	/*0x000*/     UINT32       OneSecured : 1;      // 25 BitPosition                 
	/*0x000*/     UINT32       MultipleSecured : 1; // 26 BitPosition                 
	/*0x000*/     UINT32       Spare : 1;           // 27 BitPosition                 
	/*0x000*/     UINT32       LongVad : 1;         // 28 BitPosition                 
	/*0x000*/     UINT32       ExtendableFile : 1;  // 29 BitPosition                 
	/*0x000*/     UINT32       Inherit : 1;         // 30 BitPosition                 
	/*0x000*/     UINT32       CopyOnWrite : 1;     // 31 BitPosition                 
}MMVAD_FLAGS2, * PMMVAD_FLAGS2;

typedef struct _MMVAD_FLAGS3              // 8 elements, 0x8 bytes (sizeof) 
{
	/*0x000*/     UINT64       PreferredNode : 6;       // 0 BitPosition                  
	/*0x000*/     UINT64       Teb : 1;                 // 6 BitPosition                  
	/*0x000*/     UINT64       Spare : 1;               // 7 BitPosition                  
	/*0x000*/     UINT64       SequentialAccess : 1;    // 8 BitPosition                  
	/*0x000*/     UINT64       LastSequentialTrim : 15; // 9 BitPosition                  
	/*0x000*/     UINT64       Spare2 : 8;              // 24 BitPosition                 
	/*0x000*/     UINT64       LargePageCreating : 1;   // 32 BitPosition                 
	/*0x000*/     UINT64       Spare3 : 31;             // 33 BitPosition                 
}MMVAD_FLAGS3, * PMMVAD_FLAGS3;

typedef struct _MMVAD                          // 15 elements, 0x78 bytes (sizeof) 
{
	union                                      // 2 elements, 0x8 bytes (sizeof)   
	{
		/*0x000*/         INT64        Balance : 2;              // 0 BitPosition                    
		/*0x000*/         struct _MMVAD* Parent;
	}u1;
	/*0x008*/     struct _MMVAD* LeftChild;
	/*0x010*/     struct _MMVAD* RightChild;
	/*0x018*/     UINT64       StartingVpn;
	/*0x020*/     UINT64       EndingVpn;
	union                                      // 2 elements, 0x8 bytes (sizeof)   
	{
		/*0x028*/         UINT64       LongFlags;
		/*0x028*/         struct _MMVAD_FLAGS VadFlags;          // 7 elements, 0x8 bytes (sizeof)   
	}u;
	/*0x030*/     union _EX_PUSH_LOCK PushLock;             // 7 elements, 0x8 bytes (sizeof)   
	union                                      // 2 elements, 0x8 bytes (sizeof)   
	{
		/*0x038*/         UINT64       LongFlags3;
		/*0x038*/         struct _MMVAD_FLAGS3 VadFlags3;        // 8 elements, 0x8 bytes (sizeof)   
	}u5;
	union                                      // 2 elements, 0x4 bytes (sizeof)   
	{
		/*0x040*/         ULONG32      LongFlags2;
		/*0x040*/         struct _MMVAD_FLAGS2 VadFlags2;        // 9 elements, 0x4 bytes (sizeof)   
	}u2;
	union                                      // 2 elements, 0x8 bytes (sizeof)   
	{
		/*0x048*/         struct _SUBSECTION* Subsection;
		/*0x048*/         struct _MSUBSECTION* MappedSubsection;
	};
	/*0x050*/     struct _MMPTE* FirstPrototypePte;
	/*0x058*/     struct _MMPTE* LastContiguousPte;
	/*0x060*/     struct _LIST_ENTRY ViewLinks;              // 2 elements, 0x10 bytes (sizeof)  
	/*0x070*/     struct _EPROCESS* VadsProcess;
}MMVAD, * PMMVAD;


typedef union _EXHANDLE
{
	struct
	{
		int TagBits : 2;
		int Index : 30;
	} u;
	void* GenericHandleOverlay;
	ULONG_PTR Value;
} EXHANDLE, * PEXHANDLE;

typedef struct _HANDLE_TABLE
{
	ULONG_PTR TableCode;
	struct _EPROCESS* QuotaProcess;
	HANDLE UniqueProcessId;
	void* HandleLock;
	struct _LIST_ENTRY HandleTableList;
	EX_PUSH_LOCK HandleContentionEvent;
	struct _HANDLE_TRACE_DEBUG_INFO* DebugInfo;
	int ExtraInfoPages;
	ULONG Flags;
	ULONG FirstFreeHandle;
	struct _HANDLE_TABLE_ENTRY* LastFreeHandleEntry;
	ULONG HandleCount;
	ULONG NextHandleNeedingPool;
	// More fields here...
} HANDLE_TABLE, * PHANDLE_TABLE;
typedef struct _HANDLE_TABLE_ENTRY // Size=16
{
	union
	{
		ULONG_PTR VolatileLowValue; // Size=8 Offset=0
		ULONG_PTR LowValue; // Size=8 Offset=0
		struct _HANDLE_TABLE_ENTRY_INFO* InfoTable; // Size=8 Offset=0
		struct
		{
			ULONG_PTR Unlocked : 1; // Size=8 Offset=0 BitOffset=0 BitCount=1
			ULONG_PTR RefCnt : 16; // Size=8 Offset=0 BitOffset=1 BitCount=16
			ULONG_PTR Attributes : 3; // Size=8 Offset=0 BitOffset=17 BitCount=3
			ULONG_PTR ObjectPointerBits : 44; // Size=8 Offset=0 BitOffset=20 BitCount=44
		};
	};
	union
	{
		ULONG_PTR HighValue; // Size=8 Offset=8
		struct _HANDLE_TABLE_ENTRY* NextFreeHandleEntry; // Size=8 Offset=8
		union _EXHANDLE LeafHandleValue; // Size=8 Offset=8
		struct
		{
			ULONG GrantedAccessBits : 25; // Size=4 Offset=8 BitOffset=0 BitCount=25
			ULONG NoRightsUpgrade : 1; // Size=4 Offset=8 BitOffset=25 BitCount=1
			ULONG Spare : 6; // Size=4 Offset=8 BitOffset=26 BitCount=6
		};
	};
	ULONG TypeInfo; // Size=4 Offset=12
} HANDLE_TABLE_ENTRY, * PHANDLE_TABLE_ENTRY;

typedef BOOLEAN(*EX_ENUMERATE_HANDLE_ROUTINE)(
#if !defined(_WIN7_)
	IN PHANDLE_TABLE HandleTable,
#endif
	IN PHANDLE_TABLE_ENTRY HandleTableEntry,
	IN HANDLE Handle,
	IN PVOID EnumParameter
	);



#endif // ! _MY_BB_HEADER

