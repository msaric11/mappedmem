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


       
        runRender(Process::state);
      
    #else
        fmt::print("Linux not detected\n");
    #endif
    
}
