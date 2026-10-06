<img src="images/banner.png">

# Saturn Anti Cheat
## Install
> [!WARNING]
> The driver currently only gets signed in my workspace
> Refer to src/driver/CmakeLists.txt:43 to fix it yourself
### Requirements
- CMake 3.28 (`winget install -e --id Microsoft.WindowsSDK`)
- Windows driver kit (`winget install Microsoft.WindowsWDK.10.0.28000`)
- just (optional, `winget install -e --id Casey.Just`)

### Clone the source code
```bash
git clone https://github.com/optccopa/anticheat
cd anticheat
```

### Build with `just`:
```bash
just configure && just run
```

### Build with `CMake`:
```bash
cmake -B build -A x64
cmake --build build --config Release
& "./build/bin/Release/SaturnAntiCheatLauncher.exe"
```

## Current Features
### Kernel driver
- Basically nothing besides loading and unloading
### Launcher
- Starts the game with -insecure
- Handles the driver; loading and unloading
- Clean saturn colored native gui

<img src="images/gui.png" style="max-width: 50%; height: auto;">
