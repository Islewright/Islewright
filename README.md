# Islewright
A 2D tile-map based building &amp; automation simulation game

## Build

Islewright uses vcpkg manifest mode for third-party dependencies. Set `VCPKG_ROOT`
to your vcpkg checkout, then configure with one of the included CMake presets:

```powershell
cmake --preset debug
cmake --build --preset debug
```
