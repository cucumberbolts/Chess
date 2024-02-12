#include "Application.h"

#include <imgui.h>

#include <glm/gtc/type_ptr.hpp>

void Application::RenderSettingsPanel(bool* show) {
    if (!*show)
        return;

    ImGuiColorEditFlags colourEditFlags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar;

    ImGui::Begin("Settings", show);

    static int currentTheme;
    static const char* themes[] = { "Dark", "Light" };

    if (ImGui::BeginTable("ColoursTable", 2, ImGuiTableFlags_SizingFixedSame)) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Theme");
        ImGui::TableNextColumn();
        if (ImGui::Combo("##Theme", &currentTheme, themes, sizeof(themes) / sizeof(const char*))) {
            if (currentTheme == 0)
                ImGui::StyleColorsDark();
            else
                ImGui::StyleColorsLight();
        }

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Background colour");
        ImGui::TableNextColumn();
        ImGui::ColorEdit4("##Background colour", glm::value_ptr(m_BackgroundColour), colourEditFlags);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Dark square colour");
        ImGui::TableNextColumn();
        ImGui::ColorEdit4("##Dark square colour", glm::value_ptr(m_DarkSquareColour), colourEditFlags);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Light square colour");
        ImGui::TableNextColumn();
        ImGui::ColorEdit4("##Light square colour", glm::value_ptr(m_LightSquareColour), colourEditFlags);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Legal move colour");
        ImGui::TableNextColumn();
        ImGui::ColorEdit4("##Legal move colour", glm::value_ptr(m_LegalMoveColour), colourEditFlags);

        ImGui::EndTable();
    }

    ImGui::End();
}
