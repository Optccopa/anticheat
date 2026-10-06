add_executable(SaturnAntiCheatLauncher WIN32 main.c launcher.rc)

set_property(TARGET SaturnAntiCheatLauncher PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_link_options(SaturnAntiCheatLauncher PRIVATE "/MANIFESTUAC:level='requireAdministrator'")