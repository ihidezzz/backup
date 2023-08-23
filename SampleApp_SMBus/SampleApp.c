/** @file
  This is a simple shell application

  Copyright (c) 2008, Intel Corporation                                                         
  All rights reserved. This program and the accompanying materials                          
  are licensed and made available under the terms and conditions of the BSD License         
  which accompanies this distribution.  The full text of the license may be found at        
  http://opensource.org/licenses/bsd-license.php                                            

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

**/

/**
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/

#include "SampleApp.h"

UINTN   Argc;
CHAR16  **Argv;

EFI_STATUS
GetArg (
  VOID
  )
{
  EFI_STATUS                     Status;
  EFI_SHELL_PARAMETERS_PROTOCOL  *ShellParameters;

  Status = gBS->HandleProtocol (
                  gImageHandle,
                  &gEfiShellParametersProtocolGuid,
                  (VOID **)&ShellParameters
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Argc = ShellParameters->Argc;
  Argv = ShellParameters->Argv;
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  gST->ConOut->OutputString(gST->ConOut, L"version:2.0\n\r");
  EFI_STATUS Status;
  UINT8 Result, Offset = 0x00;
  UINT16 MemorySize;
  UINT8 MemAddr;
  Status = GetArg ();
  if (EFI_ERROR (Status)) {
    Print (L"Please use UEFI SHELL to run this application!\n", Status);
    return Status;
  }

  Print(L"%d\n", Argc);
  gST->ConOut->OutputString(gST->ConOut, Argv[1]);
  Print(L"\n");
  

  if (StrnCmp(Argv[1], L"0x", 2) != 0)
  {
    Print (L"Memory Address input error, please input 0x**\n");
    return EFI_SUCCESS;
  }

  MemAddr = (UINT8)(StrHexToUintn(Argv[1]) & 0x0000FF);
  Print(L"0x%x\n", MemAddr);
  
  for (UINT16 i = 0; i <= 255; i++)
  {
    Result = ByteReadSpd(MemAddr, Offset);
    Print(L"0x%02X ", Result);
    if ((i + 1) % 16 == 0)
    {
      gST->ConOut->OutputString(gST->ConOut, L"\n\r");
    }
    Offset++;
  }
  
  GetMemoryType(MemAddr);
  MemorySize = GetMemorySize(MemAddr);
  Print(L"memory size:%dMB\n", MemorySize);

  return EFI_SUCCESS;
}

UINT8 GetMemoryAddress (
  UINT8 MemoryAddr
  )
{
  UINT8 MemoryAddress;
  MemoryAddress = MemoryAddr | 0x01;
  return MemoryAddress;
}

UINT8 ByteReadSpd (
  UINT8 MemoryAddr,
  UINT8 Offset
  )
{
  UINT8 Result, MemoryAddress;
  MemoryAddress = GetMemoryAddress(MemoryAddr);
  IoWrite8(SPD_BASEADDRESS + SPD_HSTS, 0xFE);
  IoWrite8(SPD_BASEADDRESS + SPD_TSA, MemoryAddress);
  IoWrite8(SPD_BASEADDRESS + SPD_HCMD, Offset);
  IoWrite8(SPD_BASEADDRESS + SPD_HSTS, 0xFE);
  IoWrite8(SPD_BASEADDRESS + SPD_HCTL, 0x48);
  gBS->Stall(5000);
  Result = IoRead8(SPD_BASEADDRESS + SPD_DATA);
  return Result;
}

EFI_STATUS GetMemoryType(
  INT8 MemoryAddr
)
{
  UINT8 Temp;
  Temp = ByteReadSpd(MemoryAddr, 0x02);
  if (Temp == 0x01)
  {
    gST->ConOut->OutputString(gST->ConOut, L"memory type:Fast Page Mode\n\r");
  } else if (Temp == 0x02)
  {
    gST->ConOut->OutputString(gST->ConOut, L"memory type:EDO\n\r");
  } else if (Temp == 0x03)
  {
    gST->ConOut->OutputString(gST->ConOut, L"memory type:Pipelined Nibble\n\r");
  } else if (Temp == 0x04)
  {
    gST->ConOut->OutputString(gST->ConOut, L"memory type:SDRAM\n\r");
  } else if (Temp == 0x05)
  {
    gST->ConOut->OutputString(gST->ConOut, L"memory type:ROM\n\r");
  } else if (Temp == 0x06)
  {
    gST->ConOut->OutputString(gST->ConOut, L"memory type:DDR SGRAM\n\r");
  } else if (Temp == 0x07)
  {
    gST->ConOut->OutputString(gST->ConOut, L"memory type:DDR SDRAM\n\r");
  } else if (Temp == 0x08)
  {
    gST->ConOut->OutputString(gST->ConOut, L"memory type:DDR2 SDRAM\n\r");
  } else if (Temp == 0x09)
  {
    gST->ConOut->OutputString(gST->ConOut, L"memory type:DDR2 SDRAM FB-DIMM\n\r");
  } else if (Temp == 0x0A)
  {
    gST->ConOut->OutputString(gST->ConOut, L"memory type:DDR2 SDRAM FB-DIMM PROBE\n\r");
  } else if (Temp == 0x0B)
  {
    gST->ConOut->OutputString(gST->ConOut, L"memory type:DDR3 SDRAM\n\r");
  } else if (Temp == 0x0C)
  {
    gST->ConOut->OutputString(gST->ConOut, L"memory type:DDR4 SDRAM\n\r");
  } else
  {
    gST->ConOut->OutputString(gST->ConOut, L"memory type:Reserved\n\r");
  }
  
  return EFI_SUCCESS;
}

UINT16 GetSDRAMCapacity (
  UINT8 MemoryAddr
)
{
  UINT8 Temp;
  UINT16 SDRAMCapacity = 0;
  Temp = ByteReadSpd(MemoryAddr, 0x04) & 0x0F;
  if (Temp == 0x00)
  {
    SDRAMCapacity = 256;
  } else if (Temp == 0x01)
  {
    SDRAMCapacity = 512;
  } else if (Temp == 0x02)
  {
    SDRAMCapacity = 1024;
  } else if (Temp == 0x03)
  {
    SDRAMCapacity = 2048;
  } else if (Temp == 0x04)
  {
    SDRAMCapacity = 4096;
  } else if (Temp == 0x05)
  {
    SDRAMCapacity = 8192;
  } else if (Temp == 0x06)
  {
    SDRAMCapacity = 16384;
  } else if (Temp == 0x07)
  {
    SDRAMCapacity = 32768;
  } else {
    gST->ConOut->OutputString(gST->ConOut, L"reserved\n\r");
  }

  return SDRAMCapacity;
}

UINT16 GetPrimaryBusWidth(
  UINT8 MemoryAddr
)
{
  UINT8 Temp;
  UINT16 PrimaryBusWidth = 0;
  Temp = ByteReadSpd(MemoryAddr, 0x0D) & 0x07;
  if(Temp == 0x00)
  {
    PrimaryBusWidth = 8;
  } else if (Temp == 0x01)
  {
    PrimaryBusWidth = 16;
  } else if (Temp == 0x02)
  {
    PrimaryBusWidth = 32;
  } else if (Temp == 0x03)
  {
    PrimaryBusWidth = 64;
  } else
  {
    gST->ConOut->OutputString(gST->ConOut, L"reserved\n\r");
  }

  return PrimaryBusWidth;
}

UINT16 GetSDRAMWidth(
  UINT8 MemoryAddr
)
{
  UINT8 Temp;
  UINT16 SDRAMWidth = 0;
  Temp = ByteReadSpd(MemoryAddr, 0x0C) & 0x07;
  if (Temp == 0x00)
  {
    SDRAMWidth = 4;
  } else if (Temp == 0x01)
  {
    SDRAMWidth = 8;
  } else if (Temp == 0x02)
  {
    SDRAMWidth = 16;
  } else if (Temp == 0x03)
  {
    SDRAMWidth = 32;
  } else
  {
    gST->ConOut->OutputString(gST->ConOut, L"reserved\n\r");
  }

  return SDRAMWidth;
}

UINT16 GetLogicalRankPerDIMM(
  UINT8 MemoryAddr
)
{
  UINT8 Temp;
  UINT16 LogicalRankPerDIMM = 0;
  Temp = ByteReadSpd(MemoryAddr, 0x0C) & 0x38;
  if (Temp == 0x00)
  {
    LogicalRankPerDIMM = 1;
  } else if (Temp == 0x08)
  {
    LogicalRankPerDIMM = 2;
  } else if (Temp == 0x10)
  {
    LogicalRankPerDIMM = 3;
  } else if (Temp == 0x18)
  {
    LogicalRankPerDIMM = 4;
  } else
  {
    gST->ConOut->OutputString(gST->ConOut, L"reserved\n\r");
  }
  
  return LogicalRankPerDIMM;
}

UINT16 GetMemorySize(
  UINT8 MemoryAddr
)
{
  UINT16 SDRAMCapacity, PrimaryBusWidth, SDRAMWidth, LogicalRankPerDIMM, Total = 0;
  SDRAMCapacity = GetSDRAMCapacity(MemoryAddr);
  PrimaryBusWidth = GetPrimaryBusWidth(MemoryAddr);
  SDRAMWidth = GetSDRAMWidth(MemoryAddr);
  LogicalRankPerDIMM = GetLogicalRankPerDIMM(MemoryAddr);

  Total = SDRAMCapacity / 8 * PrimaryBusWidth / SDRAMWidth * LogicalRankPerDIMM;
  return Total;
}
