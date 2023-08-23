  EFI_STATUS Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL *gGraphicsOutput;

  UINT32 frameWidth, frameHeight;

  EFI_GRAPHICS_OUTPUT_BLT_PIXEL ColorBuf[1000];
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL BackGroundColor = {192, 0, 0, 0};

  Status = gBS->LocateProtocol(
    &gEfiGraphicsOutputProtocolGuid,
    NULL,
    &gGraphicsOutput
  );

  frameWidth = (UINT32)gGraphicsOutput->Mode->Info->HorizontalResolution;
  frameHeight = (UINT32)gGraphicsOutput->Mode->Info->VerticalResolution;

  Print(L"frameWidth is %d, frameHeight is %d\n", frameWidth, frameHeight);

  Status = gGraphicsOutput->Blt(
    gGraphicsOutput,
    &BackGroundColor,
    EfiBltVideoFill,
    0, 0, frameWidth / 2, frameHeight / 2, frameWidth / 2, frameHeight / 2, 0);

  gBS->SetMem(ColorBuf, 1000 * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0);
  Status = gGraphicsOutput->Blt(
    gGraphicsOutput,
    ColorBuf,
    EfiBltBufferToVideo,
    0, 0, 0, 0, frameWidth / 2, frameHeight / 2,
    100 * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
  );

  Status = gGraphicsOutput->Blt(
    gGraphicsOutput,
    ColorBuf,
    EfiBltBufferToVideo,
    0, 5, 20, 60, 100, 5,
    100 * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
  );

  Status = gGraphicsOutput->Blt(
    gGraphicsOutput,
    0,
    EfiBltVideoToVideo,
    20, 20, 200, 60,
    100, 10, 0
  );

  Status = gGraphicsOutput->Blt(
    gGraphicsOutput,
    ColorBuf,
    EfiBltVideoToBltBuffer,
    20, 20, 0, 0, 100, 10,
    100 * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
  );