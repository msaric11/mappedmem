#include "hello_imgui/hello_imgui.h"
#include "imgui.h"
#include "proc.hpp"
#include "app.hpp"
#include <sys/types.h>


using namespace Process;


void renderProcSelect(const ProcsState& state) {

    const auto& procs = state.procs;

    const float openWidth = 200.0f;
    ImGui::BeginChild("Proc select", ImVec2(openWidth, 0.0f), ImGuiChildFlags_Borders);
    for (const auto& [pid, proc] : procs) {
        std::string procLabel = proc.procName + "-" + std::to_string(pid);
        if (ImGui::TreeNode(procLabel.c_str())) {
            for (size_t i = 0; i < proc.memRegions.size(); ++i) {
                const auto& region = proc.memRegions[i];
                std::string regionLabel = std::to_string(region.start) + " - " + region.name.c_str();
                if (ImGui::Selectable(regionLabel.c_str())) {
                    Process::selectProc(pid);
                    Process::selectRegion(i);
                }
            }
            ImGui::TreePop();
        }
    }
    ImGui::EndChild();
}

void renderMemTable(const ProcsState& state) {

    const auto& memBuffers = state.memBuffers;
    const auto& proc = state.procs.find(state.selectedProc)->second;    // .find() here assumes proc always exists
    const size_t regionIdx = state.selectedRegion;

    if (memBuffers.empty() || regionIdx >= memBuffers.size() || regionIdx >= proc.memRegions.size()) {
        ImGui::Text("Select a process");
        return;
    }
    constexpr size_t CACHE_LINE_SIZE = 64;
    static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;

    const auto& region = proc.memRegions[regionIdx];
    auto& buffer = memBuffers[regionIdx];
    std::string tableLabel = proc.procName + " - " + region.name;
    if (ImGui::BeginTable(tableLabel.c_str(), 2, flags)) {
        ImGui::TableSetupColumn("Address");
        ImGui::TableSetupColumn("Data");
        ImGui::TableHeadersRow();

        

        const char* data = buffer.get();
        size_t sz = region.end - region.start;

        for (size_t j = 0; j < sz; j += CACHE_LINE_SIZE) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("0x%lx", region.start + j);
            for (size_t k = 0; k < CACHE_LINE_SIZE && (j + k) < sz; k += 8) {
                ImGui::TableSetColumnIndex(1);
                uint64_t value{};
                size_t remaining = sz - (j + k);
                size_t n = (remaining < 8) ? remaining : 8;
                memcpy(&value, data + j + k, n);
                ImGui::Text("%lx ", value);

            }
        }
    ImGui::EndTable();
    }
}

void runRender(const ProcsState& state) {
    HelloImGui::SimpleRunnerParams params;
    params.windowTitle = "mappedmem";
    params.iniDisable = true;
    params.guiFunction = [&]() {
        renderProcSelect(state);
        ImGui::SameLine();
        renderMemTable(state);
    };
    HelloImGui::Run(params);
}
