#ifndef _SAMPLE_APP_H_
#define _SAMPLE_APP_H_

#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/IoLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/ShellParameters.h>
#include <Library/BaseLib.h>


#define SPD_BASEADDRESS   0xEFA0
#define SPD_HSTS          0x00               //Host Status
#define SPD_HCTL          0x02               //Host Control
#define SPD_HCMD          0x03               //Host Command
#define SPD_TSA           0x04               //Transmit Slave Address Register
#define SPD_DATA          0x05               //result

UINT8 GetMemoryAddress(UINT8 MemoryAddr);
UINT8 ByteReadSpd(UINT8 MemoryAddr, UINT8 Offset);
EFI_STATUS GetMemoryType(INT8 MemoryAddr);
UINT16 GetSDRAMCapacity(UINT8 MemoryAddr);
UINT16 GetPrimaryBusWidth(UINT8 MemoryAddr);
UINT16 GetSDRAMWidth(UINT8 MemoryAddr);
UINT16 GetLogicalRankPerDIMM(UINT8 MemoryAddr);
UINT16 GetMemorySize(UINT8 MemoryAddr);

#endif