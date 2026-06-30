#include <fmt/format.h>
#include "proc.hpp"
#include <iostream>
#include "hello_imgui/hello_imgui.h"
#include "app.hpp"
#include "imgui.h"

int main() {
    #if defined( __linux__)
        Process::collectProcNames();
        Process::parseMaps();
        Process::cleanMaps();

        for (const auto& [pid, proc] : Process::procs) {
            fmt::print("{} - pid: {}", proc.procName, pid);
        }
        uint32_t choice {};
        fmt::print("Select a proc: ");
        std::cin >> choice;

        const auto proc = Process::procs[choice];
        const auto mem_dump = Process::readProcMem(choice);
        if (mem_dump) {
            Process::cleanMaps();
            HelloImGui::SimpleRunnerParams params;
            params.windowTitle = proc.procName;
            params.iniDisable = true;
            params.guiFunction = [&]() { renderMemTable(mem_dump, proc); };
            HelloImGui::Run(params);
        }

      
    #else
        fmt::print("Linux not detected\n");
    #endif
    
}
