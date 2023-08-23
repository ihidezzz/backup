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
#include <Library/BaseLib.h>

EFI_STATUS ReadTempFanSpeed();
UINT16 HextoDec(UINT16 Data);
UINTN powMyfunc(UINTN a, UINTN b);

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
//  gST->ConOut->OutputString(gST->ConOut, L"version:1.0\n\r");
  EFI_STATUS Status;
  UINTN EventIndex;
  EFI_EVENT MyEvent;
  EFI_INPUT_KEY Key;
  CHAR16 Exit = 'A';

  Status = gBS->CreateEvent(EVT_TIMER | EVT_NOTIFY_SIGNAL, TPL_CALLBACK,
            (EFI_EVENT_NOTIFY)ReadTempFanSpeed, (VOID*)L"Time Out!", &MyEvent);

  Status = gBS->SetTimer(MyEvent, TimerPeriodic, 1000 * 1000);

  while (1)
  {
    gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &EventIndex);
    Status = gST->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key);
    Exit = Key.UnicodeChar;
    
    if(Exit == 'q')
    {
      break;
    }
  }

  Status = gBS->CloseEvent(MyEvent);
  Print(L"\n");
  return EFI_SUCCESS;
}

EFI_STATUS ReadTempFanSpeed()
{
  UINT8 BaseAddrH, BaseAddrL;
  UINT8 CPUFanSpeedH = 0, CPUFanSpeedL = 0;
  UINT8 PowerFanSpeedH = 0, PowerFanSpeedL = 0;
  UINT8 CPUTemp = 0;
  UINT16 HMAddr, CPUFanSpeed = 0, PowerFanSpeed = 0;

  IoWrite8(0x2E, 0x07);
  IoWrite8(0x2F, 0x08);
  IoWrite8(0x2E, 0x60);
  BaseAddrH = IoRead8(0x2F);
  IoWrite8(0x2E, 0x61);
  BaseAddrL = IoRead8(0x2F);

  HMAddr = (BaseAddrH << 8) + BaseAddrL;

  IoWrite8(HMAddr + 0xFF, 0x01);
  gBS->Stall(500);
  CPUTemp = IoRead8(HMAddr + 0x50);

  IoWrite8(HMAddr + 0xFF, 0x05);
  gBS->Stall(500);
  CPUFanSpeedL = IoRead8(HMAddr + 0x04);
  CPUFanSpeedH = IoRead8(HMAddr + 0x05);
  PowerFanSpeedL = IoRead8(HMAddr + 0x08);
  PowerFanSpeedH = IoRead8(HMAddr + 0x09);
  CPUFanSpeed =(CPUFanSpeedH << 8) + CPUFanSpeedL;
  PowerFanSpeed = (PowerFanSpeedH << 8) + PowerFanSpeedL;

  Print(L"CPU temperature:%d CPU Fan Speed:%4d Power Fan Speed:%4d\r", CPUTemp, CPUFanSpeed, PowerFanSpeed);

  return EFI_SUCCESS;
}
















