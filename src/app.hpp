
#pragma once
#include "proc.hpp"
#include <sys/types.h>


void renderProcSelect(const Process::ProcsState& state);
void renderMemTable(const Process::ProcsState& state);
void runRender(const Process::ProcsState& state);
