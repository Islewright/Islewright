# Islewright
A 2D tile-map based building &amp; automation simulation game

## Build

Islewright uses vcpkg manifest mode for third-party dependencies. On a fresh
machine, clone vcpkg, run its bootstrap script, then set `VCPKG_ROOT` to that
checkout before configuring with one of the included CMake presets:

```powershell
git clone https://github.com/microsoft/vcpkg.git
.\vcpkg\bootstrap-vcpkg.bat
$env:VCPKG_ROOT = "$PWD\vcpkg"
```

```powershell
cmake --preset debug
cmake --build --preset debug
```
