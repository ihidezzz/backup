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

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>

UINT8 ConvertType(CHAR16 c);

UINT32 GetVenID(UINT32 *Data);
UINT32 GetDevID(UINT32 *Data);
UINT32 GetClassCode(UINT32 *Data);
EFI_STATUS GetHeaderType(UINT32 *Data);

EFI_STATUS UefiMain (IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE * SystemTable)
{
	gST->ConOut->OutputString(gST->ConOut, L"version:1.2\n\r");


	EFI_STATUS Status;
  EFI_HANDLE  *HandleBuffer;
  UINTN HandleCount;
  EFI_PCI_IO_PROTOCOL *PciDevice;

  UINTN EventIndex;
  EFI_INPUT_KEY Key;
  CHAR16 StrBuffer[10] = {0};

  UINT32 Bus, Dev, Func;
    
  UINTN SegmentNumber;
  UINTN BusNumber;
  UINTN DeviceNumber;
  UINTN FunctionNumber;

	UINT32 Data[64];

  //获取所有PCI设备的句柄
  Status=gBS->LocateHandleBuffer(
    ByProtocol,
    &gEfiPciIoProtocolGuid,
    NULL,
    &HandleCount,
    &HandleBuffer
  );
  
  if(EFI_ERROR(Status))
  {
    return Status;
  }

  for (int i = 0; i < HandleCount; i++)
  {
    Status = gBS->HandleProtocol(
      HandleBuffer[i],
      &gEfiPciIoProtocolGuid,
      &PciDevice
    );

    if (EFI_ERROR(Status))
    {
      continue;
    }

    Status = PciDevice->GetLocation(
      PciDevice,
      &SegmentNumber,
      &BusNumber,
      &DeviceNumber,
      &FunctionNumber
    );

    if (EFI_ERROR(Status))
    {
      continue;
    }

    Print(L"BDF:0x%04X:0x%02X:0x%02X:0x%02X\n", 
    SegmentNumber, BusNumber, DeviceNumber, FunctionNumber);
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

  Print(L"%X %X %X\n", Bus, Dev, Func);

  for (int i = 0; i < HandleCount; i++)
  {
    Status = gBS->HandleProtocol(
      HandleBuffer[i],
      &gEfiPciIoProtocolGuid,
      &PciDevice
    );

    if (EFI_ERROR(Status))
    {
      continue;
    }

    Status = PciDevice->GetLocation(
      PciDevice,
      &SegmentNumber,
      &BusNumber,
      &DeviceNumber,
      &FunctionNumber
    );

    if (EFI_ERROR(Status))
    {
      continue;
    }

    if ((Bus == BusNumber) && (Dev == DeviceNumber) && (Func == FunctionNumber))
    {
      Status = PciDevice->Pci.Read(
        PciDevice,
        EfiPciIoWidthUint32,
        0x00,
        64,
        Data
      );

      if (EFI_ERROR(Status))
      {
        continue;
      }

      for (int j = 0; j < 64; j++)
      {
        Print(L"0x%08X ", Data[j]);
		    if ((j + 1) % 4 == 0)
		    {
			    Print(L"\n");
		    }
      }
      Print(L"\n");

      Print(L"VendorID:0x%04X\n", GetVenID(Data));
      Print(L"DeviceID:0x%04X\n", GetDevID(Data));
      Print(L"ClassCode:0x%06X\n", GetClassCode(Data));
      GetHeaderType(Data);
      Print(L"\n");
      gBS->FreePool(HandleBuffer);
      return  EFI_SUCCESS;;
    }else
    {
      continue;
    }
  }
  
  Print(L"Not found the PCI Device\n");
  gBS->FreePool(HandleBuffer);
	return  EFI_SUCCESS;
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
  case 'a':
    Result = 10;
    break;
  case 'b':
    Result = 11;
    break;
  case 'c':
    Result = 12;
    break;
  case 'd':
    Result = 13;
    break;
  case 'e':
    Result = 14;
    break;
  case 'f':
    Result = 15;
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

UINT32 GetVenID(
  UINT32 *Data
)
{
  UINT32 Result;
  Result = Data[0] & 0x0000FFFF;
  return Result;
}

UINT32 GetDevID(
  UINT32 *Data
)
{
  UINT32 Result;
  Result = Data[0] >> 16;
  return Result;
}

UINT32 GetClassCode(
  UINT32 *Data
)
{
  UINT32 Result;
  Result = Data[2] >> 8;
  return Result;
}

EFI_STATUS GetHeaderType(
  UINT32 *Data
)
{
  UINT32 Result;
  Result = (Data[3] >> 16) & 0x0003;

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
