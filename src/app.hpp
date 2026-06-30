
#pragma once
#include "proc.hpp"
#include <sys/types.h>


void renderMemTable(const std::optional<std::vector<std::unique_ptr<char[]>>>& memBuffers, const Process::Proc& proc);

