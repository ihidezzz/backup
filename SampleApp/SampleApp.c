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
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/SmbusHc.h>
#include <Library/SmbusLib.h>
#include <Library/UefiLib.h>
#include <Protocol/ShellParameters.h>
#include <Library/BaseLib.h>

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
  EFI_STATUS Status;
  EFI_SMBUS_HC_PROTOCOL *Smbus = NULL;
  UINT64 SmbusAddress;
  EFI_SMBUS_DEVICE_ADDRESS SmbusDeviceAddress;
  UINTN Length = 1;
  UINT8 Data[256] = {0};
  UINT16 SpdTemp = 0;
  UINT16 result = 0;
  UINT16 result1 = 0;
  UINT16 result2 = 0;


  Status = GetArg ();

  if (Argc == 1)
  {
    Print (L"Memory Address input is NULL, please input Parameters\n");
    return EFI_SUCCESS;
  }
  
  if (StrnCmp(Argv[1], L"0x", 2) != 0)
  {
    Print (L"Memory Address input error, please input 0x**\n");
    return EFI_SUCCESS;
  }

  SmbusAddress = (UINT8)(StrHexToUintn(Argv[1]) & 0x0000FF);
  SmbusDeviceAddress.SmbusDeviceAddress = SmbusAddress >> 1;

  Status = gBS->LocateProtocol (
    &gEfiSmbusHcProtocolGuid, 
    NULL, 
    &Smbus);

  for(int i = 0; i < 256; i++)
  {
    Status = Smbus->Execute (
    Smbus,
    SmbusDeviceAddress,
    i,
    EfiSmbusReadByte,
    FALSE,
    &Length,
    &Data[i]
    );
  }

  for (int i = 0; i < 256; i++)
  {
    Print(L"0x%2X ", Data[i]);
    if ((i + 1) % 16 == 0)
    {
      Print(L"\n");
    }
  }
  
  SpdTemp = (((UINT16)Data[50] << 8) + Data[49]) >> 2;
  
  if (SpdTemp & 0x0400)
  {
    result = (~(SpdTemp & 0x03FF) + 1) * 25;
  }else
  {
    result = (SpdTemp & 0x03FF) * 25;
  }

  result1 = result % 100;
  result2 = result / 100;
  Print(L"SPD temperature is %d.%d\n", result2, result1);
  
  return EFI_SUCCESS;
}

















