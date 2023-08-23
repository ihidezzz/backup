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

#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/IoLib.h>
#include <Library/UefiBootServicesTableLib.h>

#define CFG_ADDRESS 0x0CF8
#define CFG_DATA 0x0CFC

UINT8 ConvertType(CHAR16 c);

UINT32 GetAddr(UINT32 Bus, UINT32 Dev, UINT32 Func, UINT32 Offset);
UINT32 GetData(UINT32 Bus, UINT32 Dev, UINT32 Func, UINT32 Offset);
UINT32 GetMemAddr(UINT32 Bus, UINT32 Dev, UINT32 Func, UINT32 Offset);
UINT32 GetDataByMem(UINT32 Bus, UINT32 Dev, UINT32 Func, UINT32 Offset);

UINT32 GetVenID(UINT32 Bus, UINT32 Dev, UINT32 Func);
UINT32 GetDevID(UINT32 Bus, UINT32 Dev, UINT32 Func);
UINT32 GetClassCode(UINT32 Bus, UINT32 Dev, UINT32 Func);
EFI_STATUS GetHeaderType(UINT32 Bus, UINT32 Dev, UINT32 Func);

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  gST->ConOut->OutputString(gST->ConOut, L"version:2.0\n\r");
  EFI_STATUS Status;
  UINTN EventIndex;
  EFI_INPUT_KEY Key;
  CHAR16 StrBuffer[10] = {0};

  UINT32 Bus, Dev, Func;
  UINT32 Data;


  for (Bus = 0; Bus < 256; Bus++)
  {
    for (Dev  = 0; Dev < 32; Dev++)
    {
      for (Func = 0; Func < 8; Func++)
      {
        if (GetData(Bus, Dev, Func, 0) != 0xFFFFFFFF)
        {
          Print(L"0x%02X 0x%02X 0x%02X\n", Bus, Dev, Func);
        }
      }
    }
  }
  Print(L"please input bus dev func:\n");

  for (int i = 0; i < 8; i++)
  {
    gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &EventIndex);
    Status = gST->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key);
    StrBuffer[i] = Key.UnicodeChar;
  }

  StrBuffer[8] = '\0';
  gST->ConOut->OutputString(gST->ConOut, StrBuffer);
  Print(L"\n");

  Bus = ConvertType(StrBuffer[1]) + ConvertType(StrBuffer[0]) * 16;
  Dev = ConvertType(StrBuffer[4]) + ConvertType(StrBuffer[3]) * 16;
  Func = ConvertType(StrBuffer[7]) + ConvertType(StrBuffer[6]) * 16;

  for(int i = 0; i < 64; i++)
  {
    Data = GetData(Bus, Dev, Func, i);
    Print(L"0x%08X ", Data);
    if((i + 1) % 4 == 0)
    {
      Print(L"\n");
    }
  }

  Print(L"\n");

  for(int i = 0; i < 64; i++)
  {
    Data = GetDataByMem(Bus, Dev, Func, i);
    Print(L"0x%08X ", Data);
    if((i + 1) % 4 == 0)
    {
      Print(L"\n");
    }
  }
  Print(L"\n");

  Print(L"VendorID:0x%04X\n", GetVenID(Bus, Dev, Func));
  Print(L"DeviceID:0x%04X\n", GetDevID(Bus, Dev, Func));
  Print(L"ClassCode:0x%06X\n", GetClassCode(Bus, Dev, Func));
  GetHeaderType(Bus, Dev, Func);
  Print(L"\n");

  return EFI_SUCCESS;
}

UINT8 ConvertType(
  CHAR16 c
)
{
  UINT8 Result;
  switch (c)
  {
  case '0':
    Result = 0;
    break;
  case '1':
    Result = 1;
    break;
  case '2':
    Result = 2;
    break;
  case '3':
    Result = 3;
    break;
  case '4':
    Result = 4;
    break;
  case '5':
    Result = 5;
    break;
  case '6':
    Result = 6;
    break;
  case '7':
    Result = 7;
    break;
  case '8':
    Result = 8;
    break;
  case '9':
    Result = 9;
    break;
  case 'A':
    Result = 10;
    break;
  case 'B':
    Result = 11;
    break;
  case 'C':
    Result = 12;
    break;
  case 'D':
    Result = 13;
    break;
  case 'E':
    Result = 14;
    break;
  case 'F':
    Result = 15;
    break;
  default:
    break;
  }

  return Result;
}

UINT32 GetAddr(
  UINT32 Bus,
  UINT32 Dev,
  UINT32 Func,
  UINT32 Offset
)
{
  UINT32 Addr;
  Addr = 0x80000000 + Bus * 0x10000 + Dev * 0x800 + Func * 0x100 + Offset * 0x4;
  return Addr;
}

UINT32 GetData(
  UINT32 Bus,
  UINT32 Dev,
  UINT32 Func,
  UINT32 Offset
)
{
  UINT32 Addr, Data;
  Addr = GetAddr(Bus, Dev, Func, Offset);
  IoWrite32(CFG_ADDRESS, Addr);
  Data = IoRead32(CFG_DATA);
  return Data;
}

UINT32 GetMemAddr(
  UINT32 Bus,
  UINT32 Dev,
  UINT32 Func,
  UINT32 Offset
)
{
  UINT32 Addr;
  Addr = (GetData(00, 00, 00, 24) & 0xFFFFFFFE) + Bus * 0x100000 + Dev * 0x8000 + Func * 0x1000 + Offset * 0x4;
  return Addr;
}

UINT32 GetDataByMem(
  UINT32 Bus,
  UINT32 Dev,
  UINT32 Func,
  UINT32 Offset
)
{
  UINT32 Addr, Data;
  Addr = GetMemAddr(Bus, Dev, Func, Offset);
  Data = MmioRead32(Addr);
  return Data;
}

UINT32 GetVenID(
  UINT32 Bus, 
  UINT32 Dev, 
  UINT32 Func
)
{
  UINT32 Result;
  Result = GetDataByMem(Bus, Dev, Func, 0) & 0x0000FFFF;
  return Result;
}

UINT32 GetDevID(
  UINT32 Bus, 
  UINT32 Dev, 
  UINT32 Func
)
{
  UINT32 Result;
  Result = GetDataByMem(Bus, Dev, Func, 0) >> 16;
  return Result;
}

UINT32 GetClassCode(
  UINT32 Bus, 
  UINT32 Dev, 
  UINT32 Func
)
{
  UINT32 Result;
  Result = GetDataByMem(Bus, Dev, Func, 2) >> 8;
  return Result;
}

EFI_STATUS GetHeaderType(
  UINT32 Bus, 
  UINT32 Dev, 
  UINT32 Func
)
{
  UINT32 Result;
  Result = (GetDataByMem(Bus, Dev, Func, 3) >> 16) & 0x0003;

  if (Result == 0x00) {
    Print(L"is PCI agent\n");
  } else if (Result == 0x01) {
    Print(L"is PCI to PCI bridge\n");
  } else if (Result == 0x02) {
    Print(L"is CardBus bridge\n");
  } else {
    Print(L"error\n");
  }

  return EFI_SUCCESS;
}
