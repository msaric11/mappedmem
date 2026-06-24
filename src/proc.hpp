
#pragma once

#include <cstdint>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include <optional>

namespace Process {

    struct MemRegion {
        uintptr_t start;      
        uintptr_t end;      
        std::string perms;
        // Path to file of pertaining to specific MemRegion of process
        std::string name;
    };

    struct Proc {
        std::string procName;
        uint32_t procId;
        std::vector<MemRegion> memRegions;
    };

    static const std::filesystem::path PROC_GLOBAL_PATH = "/proc/";
    extern std::unordered_map<uint32_t, Proc> procs;


    // Check if dir is a proc based on whether it converts to int. Extract pid in the process
    bool dirIsProc(const std::filesystem::directory_entry& entry, Proc& proc);
    // Iterate (non-recursively) through current system processes within /proc to find proc name within /proc/{pid}/comm
    void collectProcNames();
    // Loop through procs map to determine current procs and access its /maps
    // Each proc's maps contains the the several vaddress ranges, perms, and other info
    // Each line is MemRegion, stored in procs list of memRegions 
    void parseMaps();
    // Remove proc entries in procs maps that have empty /mem files
    void cleanMaps();
    // Use process_vm_readv syscall to access and read the registers of the given process
    std::optional<std::vector<std::unique_ptr<char[]>>> readProcMem(const uint32_t& processId);
    void printCacheLines(const char* buffer, size_t size, uintptr_t base_address);
}
