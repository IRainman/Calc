#pragma once

// define Windows version requirements
#define _WIN32_WINNT _WIN32_WINNT_WIN7 // the minimal version of OS supported by application is Windows 7.
#define _WIN32_IE_ _WIN32_IE_IE110 // it's a maximum available version of IE available since Windows 7.
#define NTDDI NTDDI_WIN8 // using NTDDI for Windows 8 for proper working with touch interfaces.
