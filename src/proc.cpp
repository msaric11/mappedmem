
#include "proc.hpp"
#include <filesystem>
#include <string>
#include <fstream>


std::vector<Process::Proc> Process::procs;

bool Process::dirIsProc(const std::filesystem::directory_entry& entry, Proc& proc) {
    try {
        proc.procId = std::stoi(entry.path().filename().string());
        return true;
    } 
    catch (...) {
        return false;
    }
}

void Process::collectProcNames() {

    for (const auto& entry : std::filesystem::directory_iterator(PROC_GLOBAL_PATH)) {
        if (!entry.is_directory()) continue;
        Process::Proc proc{};
        if (Process::dirIsProc(entry, proc)) {
            std::filesystem::path comm_path = entry.path() / "comm";
            if (std::filesystem::exists(comm_path)) {
                std::ifstream comm_file(comm_path);
                std::string comm_name {};
                if (std::getline(comm_file, comm_name)) {
                   proc.procName = comm_name; 
                }
            }
            procs.push_back(proc);
        }
    }
}
