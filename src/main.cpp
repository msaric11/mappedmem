#include <fmt/format.h>
#include "proc.hpp"
#include <iostream>

int main() {
    Process::collectProcNames();
    Process::parseMaps();
    Process::cleanMaps();

    for (const auto& [pid, proc] : Process::procs) {
        fmt::print("{} - pid: {}", proc.procName, pid);
    }
    uint32_t choice {};
    fmt::print("Select a proc: ");
    std::cin >> choice;

    auto mem_dump = Process::readProcMem(choice);
    const auto& proc = Process::procs[choice];
    size_t size {};
    if (mem_dump) {
        size = mem_dump->size();
        Process::cleanMaps();
        for (size_t i = 0; i < 1; ++i) {    // Set to 1 region limit for now
            const auto& mr = proc.memRegions[i];
            size_t region_size = mr.end - mr.start;

            fmt::print("\n--- Dumping Region: {} [0x{:X} - 0x{:X}] ---\n", 
                    mr.name, mr.start, mr.end);
            auto& buffer = (*mem_dump)[i];
            Process::printCacheLines(buffer.get(), region_size, mr.start);
        }
    }
}
