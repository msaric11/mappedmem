
#pragma once
#include "proc.hpp"
#include <sys/types.h>

extern Process::ProcsState procs;

void renderProcSelect(const Process::ProcsState& procs);
void renderMemTable(const std::vector<std::unique_ptr<char[]>>& memBuffers, const Process::Proc& proc);
void runRender();
