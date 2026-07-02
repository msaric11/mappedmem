
#include "proc.hpp"
#include <filesystem>
#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <array>
#include <sys/uio.h>
#include <optional>
#include <fmt/format.h>
#include <vector>

#if defined(__linux__)
    using namespace Process;
    
    Process::ProcsState Process::state;

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
                } else { continue; }
                state.procs[proc.procId] = proc;
            } else { continue; }
        }
    }

    void Process::parseMaps() {
        for (auto& [pid, proc] : state.procs) {
            std::string pid_string = std::to_string(pid);
            std::filesystem::path maps_path = PROC_GLOBAL_PATH / pid_string  / "maps";

            if (std::filesystem::exists(maps_path)) {
                std::ifstream maps_file(maps_path);
                std::string line {};

                while (std::getline(maps_file, line)) {
                    std::stringstream ss(line);
                    std::string range, perms, offset, dev, inode;
                    // Default name in the case MemRegion does not have a name
                    std::string name = "[anon]";
                    // Skip to next line if line is incomplete or missing
                    if (!(ss >> range >> perms >> offset >> dev >> inode)) continue;
                    ss >> std::ws;
                    
                    if (!ss.eof()) std::getline(ss, name);

                    // Filter out memory that isn't readbale
                    if (perms.empty() || perms[0] != 'r') continue;

                    MemRegion mem_region{};
                    auto split = range.find('-');
                    mem_region.start = std::stoull(range.substr(0, split), nullptr, 16);
                    mem_region.end = std::stoull(range.substr(split + 1), nullptr, 16);
                    mem_region.perms = perms;
                    mem_region.name = name;
                    proc.memRegions.push_back(mem_region);
                }
            }
        }
    }

    void Process::cleanMaps() {
        // Have to use iterator because proc entry cannot be removed while looping through it
        for (auto it = state.procs.begin(); it != state.procs.end(); ) {
            if (it->second.memRegions.empty()) {
                it = state.procs.erase(it);
            }
            else { ++it; }
        }
    }

    std::optional<std::vector<std::unique_ptr<char[]>>> Process::readProcMem(const uint32_t& processId) {
        // Changed proc to reference instead of copying data
        Process::Proc& proc = state.procs[processId];

        
        size_t size = proc.memRegions.size();
        if (size == 0) {
            std::cerr << "Error: memRegions empty" << std::endl;
            return std::nullopt;
        }
        auto local_iov = std::make_unique<iovec[]>(size);
        auto remote_iov = std::make_unique<iovec[]>(size);
        // Allocate a vec of buffers to store memRegions register data
        std::vector<std::unique_ptr<char[]>> buffers {};
        buffers.reserve(size);

        int count = 0;
        for (auto& mr : proc.memRegions) {
            size_t len = mr.end - mr.start;
            remote_iov[count].iov_base = (void *)mr.start; 
            remote_iov[count].iov_len = len; 
            buffers.push_back(std::make_unique<char[]>(len));
            // local_iov points to the buffers so that process_vm_readv can send the data
            // to the buffers
            local_iov[count].iov_base = buffers.back().get();
            local_iov[count].iov_len = len;

            count++;
        }

        ssize_t read = process_vm_readv(processId, local_iov.get(), size, remote_iov.get(), size, 0);
        
        if (read < 0) {
            std::cerr << "Error: " << std::strerror(errno);
            return std::nullopt;
        }
        
        return buffers;
    }

    void Process::printCacheLines(const char* buffer, size_t size, uintptr_t base_address) {
        const size_t CACHE_LINE_SIZE = 64; // 64 Bytes per line

        // Iterate through the buffer in 64-byte steps
        for (size_t i = 0; i < size; i += CACHE_LINE_SIZE) {
            
            // 1. Print the starting memory address of this specific cache line
            fmt::print("0x{:016X} | ", base_address + i);

            // 2. Iterate through the 64-byte block in 8-byte (64-bit) chunks
            for (size_t j = 0; j < CACHE_LINE_SIZE && (i + j) < size; j += 8) {
                
                // Check if we have a full 8 bytes left to read
                if (i + j + 8 <= size) {
                    // Safely cast the next 8 bytes into an unsigned 64-bit integer
                    uint64_t chunk = *reinterpret_cast<const uint64_t*>(buffer + i + j);
                    fmt::print("{:016X} ", chunk);
                } else {
                    // If the memory region ends abruptly, print padding
                    fmt::print("{:<16} ", "........");
                }
            }
            fmt::print("\n");
        }
    }
#endif
