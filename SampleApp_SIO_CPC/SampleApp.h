#ifndef _SAMPLE_APP_H_
#define _SAMPLE_APP_H_

#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/IoLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>

typedef struct fanSpeed
{
    UINT16 SysFanSpeed;
    UINT16 CpuFanSpeed;
}fanSpeed;

EFI_STATUS EnterMBPnP();
EFI_STATUS ExitMBPnP();
fanSpeed GetFanSpeed();

#endif