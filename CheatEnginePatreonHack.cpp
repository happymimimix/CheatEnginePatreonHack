// write_lastcheck.cpp
// Compile: cl /EHsc write_lastcheck.cpp   (MSVC)
// Or with MinGW: g++ -municode -o write_lastcheck.exe write_lastcheck.cpp

#include <windows.h>
#include <iostream>
#include <iomanip>

int main() {
    // 1) Get local SYSTEMTIME
    SYSTEMTIME stLocal;
    GetLocalTime(&stLocal); // local time (with DST)

    // Convert SYSTEMTIME -> struct tm (mktime expects local time)
    std::tm tmLocal = {};
    tmLocal.tm_year = stLocal.wYear - 1900;
    tmLocal.tm_mon = stLocal.wMonth - 1;
    tmLocal.tm_mday = stLocal.wDay;
    tmLocal.tm_hour = stLocal.wHour;
    tmLocal.tm_min = stLocal.wMinute;
    tmLocal.tm_sec = stLocal.wSecond;
    tmLocal.tm_isdst = -1; // allow mktime to determine DST

    // 2) mktime interprets tm as local time -> returns time_t (seconds since epoch UTC)
    std::time_t epochSeconds = std::mktime(&tmLocal);
    DWORD timestamp = static_cast<DWORD>(epochSeconds);

    // Minus 4 hours from the time (I wonder why?)
    timestamp -= 1<<18;

    // XOR encode with the same constant used by Cheat Engine's pcheck.dll
    const DWORD XOR_KEY = 0xE81289CD;
    DWORD encoded = timestamp ^ XOR_KEY;

    // Create/Open registry key HKCU\Software\CE
    HKEY hKey = nullptr;
    LONG result = RegCreateKeyExW(
        HKEY_CURRENT_USER,
        L"Software\\CEPatreon",
        0,
        nullptr,
        REG_OPTION_NON_VOLATILE,
        KEY_SET_VALUE,
        nullptr,
        &hKey,
        nullptr
    );

    if (result != ERROR_SUCCESS) {
        std::cerr << "RegCreateKeyEx failed: " << result << "\n";
        Sleep(500);
        return 1;
    }

    // Write the DWORD value "lastcheck"
    result = RegSetValueExW(
        hKey,
        L"lastcheck",
        0,
        REG_DWORD,
        reinterpret_cast<const BYTE*>(&encoded),
        sizeof(encoded)
    );

    if (result != ERROR_SUCCESS) {
        std::cerr << "RegSetValueEx failed: " << result << "\n";
        RegCloseKey(hKey);
        Sleep(500);
        return 1;
    }

    std::cout << "Wrote encoded timestamp value 0x"
        << std::hex << std::uppercase << encoded
        << " to HKCU\\Software\\CEPatreon\\lastcheck" << std::endl;

    // Write a random non-blank value to "cesession"
    const wchar_t* cesessionValue = L"0";
    result = RegSetValueExW(
        hKey,
        L"cesession",
        0,
        REG_SZ,
        reinterpret_cast<const BYTE*>(cesessionValue),
        (DWORD)((wcslen(cesessionValue) + 1) * sizeof(wchar_t))
    );

    if (result != ERROR_SUCCESS) {
        std::cerr << "RegSetValueEx failed: " << result << "\n";
        RegCloseKey(hKey);
        Sleep(500);
        return 1;
    }

    RegCloseKey(hKey);
    Sleep(500);
    return 0;
}
