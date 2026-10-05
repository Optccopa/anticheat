runui:
    clang src/loader/main.c -o bin/ui.exe -luser32 -lkernel32 -lgdi32
    ./bin/ui.exe
    