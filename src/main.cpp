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

        for (const auto& [pid, proc] : Process::state.procs) {
            fmt::print("{} - pid: {}", proc.procName, pid);
        }
        uint32_t choice {};
        fmt::print("Select a proc: ");
        std::cin >> choice;
        Process::state.selectedProc = choice;

        const Process::Proc& proc = Process::state.procs[Process::state.selectedProc];
        auto mem_dump = Process::readProcMem(choice);
        if (mem_dump) {
            Process::state.memBuffers = std::move(*mem_dump);
            Process::cleanMaps();
            runRender();
        }

      
    #else
        fmt::print("Linux not detected\n");
    #endif
    
}
