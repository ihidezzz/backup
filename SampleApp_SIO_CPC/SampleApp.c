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

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  gST->ConOut->OutputString(gST->ConOut, L"version:2.0\n\r");
  fanSpeed FanSpeed;
  EnterMBPnP();
  FanSpeed = GetFanSpeed();
  ExitMBPnP();
  Print(L"System fan speed is %d RPM\n", FanSpeed.SysFanSpeed);
  Print(L"Cpu fan speed is %d RPM\n", FanSpeed.CpuFanSpeed);
  return EFI_SUCCESS;
}

EFI_STATUS
EnterMBPnP(
  )
{
  IoWrite8(0x2E, 0x87);
  IoWrite8(0x2E, 0x01);
  IoWrite8(0x2E, 0x55);
  IoWrite8(0x2E, 0x55);

  return EFI_SUCCESS;
}

EFI_STATUS
ExitMBPnP(
  )
{
  IoWrite8(0x2E, 0x02);
  IoWrite8(0x2F, 0x02);

  return EFI_SUCCESS;
}

fanSpeed
GetFanSpeed(
  )
{
  UINT8 BaseAddrH, BaseAddrL;
  UINT16 SysFanSpeedH, SysFanSpeedL;
  UINT16 CpuFanSpeedH, CpuFanSpeedL;
  UINT16 EcAddr, EcData;
  fanSpeed FanSpeed;

  IoWrite8(0x2E, 0x07);
  IoWrite8(0x2F, 0x04);
  IoWrite8(0x2E, 0x60);
  BaseAddrH = IoRead8(0x2F);
  IoWrite8(0x2E, 0x61);
  BaseAddrL = IoRead8(0x2F);

  EcAddr = (BaseAddrH << 8) + BaseAddrL + 0x05;
  EcData = (BaseAddrH << 8) + BaseAddrL + 0x06;

  IoWrite8(EcAddr, 0x18);
  SysFanSpeedH = IoRead8(EcData);
  Print(L"SysFanSpeedH:0x%x\n", SysFanSpeedH);
  IoWrite8(EcAddr, 0x0D);
  SysFanSpeedL = IoRead8(EcData);
  Print(L"SysFanSpeedL:0x%x\n", SysFanSpeedL);
  IoWrite8(EcAddr, 0x19);
  CpuFanSpeedH = IoRead8(EcData);
  Print(L"CpuFanSpeedH:0x%x\n", CpuFanSpeedH);
  IoWrite8(EcAddr, 0x0E);
  CpuFanSpeedL = IoRead8(EcData);
  Print(L"CpuFanSpeedL:0x%x\n", CpuFanSpeedL);

  FanSpeed.SysFanSpeed = (UINT16)(1350000 / (((SysFanSpeedH << 8) + SysFanSpeedL) * 2));
  Print(L"SysFanSpeed:%d\n", FanSpeed.SysFanSpeed);
  FanSpeed.CpuFanSpeed = (UINT16)(1350000 / (((CpuFanSpeedH << 8) + CpuFanSpeedL) * 2));
  Print(L"CpuFanSpeed:%d\n", FanSpeed.CpuFanSpeed);

  return FanSpeed;
}







