configure:
    powershell -Command "if (Test-Path 'build') { Remove-Item -Path 'build' -Recurse -Force }"
    cmake -B build -A x64

build config="Release":
    cmake --build build --config {{config}}

run config="Release":
    cmake --build build --config {{config}}
    powershell -Command "./build/bin/{{config}}/SaturnAntiCheatLauncher.exe"
    