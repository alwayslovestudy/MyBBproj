#pragma once

#ifndef  _MY_BB_HEADER
#define _MY_BB_HEADER
#include<ntddk.h>


#define DPRINT(format, ...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, format, __VA_ARGS__)
#define BB_DEVICE_NAME "MyBBDevice"
#define BB_SYM_LNK_NAME "symBBDevice"




#endif // ! _MY_BB_HEADER

