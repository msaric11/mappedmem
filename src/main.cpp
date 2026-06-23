#include <fmt/format.h>
#include <fstream>
#include <filesystem>
#include "proc.hpp"


int main() {
    Process::collectProcNames();
    for (auto& proc : Process::procs) {
        fmt::print("{}\n", proc.procId);
        fmt::print("{}\n", proc.procName);
    }
}

