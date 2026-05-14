#include <iostream>
#include <filesystem>
#include "kioskserver.h"

#if defined(_WIN32)
    #include <windows.h>
#elif defined(__APPLE__)
    #include <mach-o/dyld.h>
    #include <limits.h>
#endif

namespace fs = std::filesystem;

static fs::path exeDir()
{
#if defined(_WIN32)
    char buf[MAX_PATH];
    GetModuleFileNameA(NULL, buf, MAX_PATH);
    return fs::path(buf).parent_path();
#elif defined(__APPLE__)
    char buf[PATH_MAX];
    uint32_t sz = sizeof(buf);
    _NSGetExecutablePath(buf, &sz);
    return fs::canonical(buf).parent_path();
#else
    return fs::canonical("/proc/self/exe").parent_path();
#endif
}

int main()
{
    // Set CWD to project root (parent of build/) so relative paths work
    // regardless of where the executable is launched from.
    fs::current_path(exeDir().parent_path());

    std::cout << "================================\n";
    std::cout << "         Conduit Server         \n";
    std::cout << "================================\n";

    KioskServer server;

    if (!server.loadConfig("config.json"))
        std::cerr << "[WARNING] config.json not found, using defaults\n";

    std::cout << "[INFO] http://localhost:8080\n";
    std::cout << "================================\n";

    server.start();
    return 0;
}
