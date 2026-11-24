# StringsTrace

Windows static file inspection based on [DIE-engine](https://github.com/horsicq/DIE-engine).

Development build — not a verified release.

## Build

Install Git, CMake, Visual Studio C++ tools and a compatible Qt SDK. From a developer PowerShell run:

```powershell
.\scripts\build.ps1 -QtPath 'C:\Qt\6.8.3\msvc2022_64'
```

Adjust the Qt path to your installation. The build downloads upstream dependencies and stages the application in `dist`.

## Current additions

- Monochrome Qt theme
- Manual ASCII string inspection with hexadecimal offsets and filtering

Inspect only files you own or have permission to analyze. A string match alone does not establish execution or cheating.

## Credits

Original engine: horsicq and DIE-engine contributors (MIT). StringsTrace modifications: reala1. Original notices and dependency licenses must accompany distributions.
