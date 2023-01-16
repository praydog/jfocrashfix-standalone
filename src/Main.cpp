#include <windows.h>

#include <utility/Module.hpp>
#include <utility/Scan.hpp>
#include <utility/Patch.hpp>

void startup_thread() {
    // Spawn a debug console
#ifdef SPAWN_CONSOLE
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
#endif

    printf("[starwarscrashfix-standalone] Patching init projected shadows!\n");

    const auto mid = utility::scan(utility::get_executable(), "c7 01 00 00 80 3f 48 8b b8 c0 1d 00 00");

    if (!mid) {
        printf("[starwarscrashfix-standalone] Failed to find init projected shadows (1)\n");
        return;
    }

    const auto func = utility::find_function_start_with_call(*mid);

    if (!func) {
        printf("[starwarscrashfix-standalone] Failed to find init projected shadows (2)\n");
        return;
    }

    static auto patch = Patch::create(*func, { 0xC3 }, true);

    printf("[starwarscrashfix-standalone] Patched init projected shadows!\n");
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason_for_call, LPVOID reserved) {
    switch (reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)startup_thread, nullptr, 0, nullptr);
        break;
    }

    return TRUE;
}