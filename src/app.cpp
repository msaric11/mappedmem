#include "hello_imgui/hello_imgui.h"
#include "imgui.h"
#include "proc.hpp"
#include "app.hpp"
#include <sys/types.h>


void renderMemTable(const std::optional<std::vector<std::unique_ptr<char[]>>>& memBuffers, const Process::Proc& proc) {

    if (memBuffers == std::nullopt) {
        ImGui::Text("Memory tables are empty");
        std::cerr << "Memory tables are empty" << "\n";
    }
    constexpr size_t CACHE_LINE_SIZE = 64;
    static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;

    if (ImGui::BeginTable("Memory Regions", 3, flags)) {
        ImGui::TableSetupColumn("Segment");
        ImGui::TableSetupColumn("Address");
        ImGui::TableSetupColumn("Data");
        ImGui::TableHeadersRow();
    }
    const size_t bufferCount = memBuffers->size();
    for (size_t i = 0; i < 1; ++i) {
        const auto& region = proc.memRegions[i];
        auto& buffer = (*memBuffers)[i];

        if (!buffer) continue;
        const char* data = buffer.get();
        size_t sz = region.end - region.start;

        for (size_t j = 0; j < sz; j += CACHE_LINE_SIZE) {
            for (size_t k = 0; k < CACHE_LINE_SIZE && (j + k) < sz; k += 8) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", region.name.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("0x%lx", region.start + j + k);
                ImGui::TableSetColumnIndex(2);
                uint64_t value{};
                memcpy(&value, data + k + j, sizeof(value));
                ImGui::Text("%lu", value);
            }
        }
    }
    ImGui::EndTable();
}
