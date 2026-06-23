
#pragma once

#include <cstdint>
#include <filesystem>
#include <vector>


namespace Process {
    struct Proc {
        std::string procName;
        uint32_t procId;
    };

    static const std::filesystem::path PROC_GLOBAL_PATH = "/proc/";
    extern std::vector<Proc> procs;

    

    // Iterate (non-recursively) through current system processes within /proc to find proc name within /proc/{pid}/comm
    void collectProcNames();
    // Check if dir is a proc based on whether it converts to int. Extract pid in the process
    bool dirIsProc(const std::filesystem::directory_entry& entry, Proc& proc);

}
