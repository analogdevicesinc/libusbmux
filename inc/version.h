/**
* Copyright (c) 2021-2022 Analog Devices, Inc. All Rights Reserved. This
* software is proprietary and confidential to Analog Devices, Inc. and its
* licensors.
*/

#define ADI_QUOTE(a) #a
#define ADI_QUOTE2(a) ADI_QUOTE(a)
#define ADI_VERSION_CONV(a, b, c, d)   ADI_QUOTE(a) "." ADI_QUOTE(b)  "." ADI_QUOTE(c) "." ADI_QUOTE(d)

#define ADI_FILE_MAJOR 1
#define ADI_FILE_MINOR 0
#define ADI_FILE_PATCH 3
#define ADI_FILE_REL   0

#define EXE_DESCRIPTION "USB MUX Library\0"
#define EXE_NAME        "libusbmux\0"
#define EXE_FILENAME    "libusbmux.dll\0"

#define EXE_VERSION ADI_VERSION_CONV(ADI_FILE_MAJOR,ADI_FILE_MINOR,ADI_FILE_PATCH,ADI_FILE_REL)
