configure:
    @clear
    powershell -Command "if (Test-Path 'build') { Remove-Item -Path 'build' -Recurse -Force }"
    cmake -B build -A x64

build config="Release":
    @clear
    cmake --build build --config {{config}}

run config="Release":
    @clear
    cmake --build build --config {{config}}
    powershell -Command "./build/bin/{{config}}/SaturnAntiCheatLauncher.exe"
