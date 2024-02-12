#include "Application.h"
#include "Utility/FileDialog.h"

#include <imgui.h>

#include <sstream>

#define GetCentre GetCenter

void Application::RenderEnginePanel(bool* show) {
    if (!*show)
        return;

    ImGui::Begin("Engine", show);

    static auto s_SelectedEngine = m_Engines.end();

    if (s_SelectedEngine == m_Engines.end()) {
        if (ImGui::Button("Create engine")) {
            ImGui::OpenPopup("Create engine");
        }

        // Always centre this window when appearing
        ImVec2 centre = ImGui::GetMainViewport()->GetCentre();
        ImGui::SetNextWindowPos(centre, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Create engine", nullptr, ImGuiWindowFlags_None)) {
            static char path[255] = { 0 };
            static char name[64] = { 0 };

            ImGui::InputText("path", path, sizeof(path));
            if (ImGui::Button("Select executable")) {
                FileDialog::Open(path, sizeof(path));

                // If the 'name' field is empty, copy the executable name to it
                if (name[0] == '\0')
                    //strcpy_s(name, sizeof(name), std::filesystem::path(path).stem().u8string().c_str());
                    strcpy(name, std::filesystem::path(path).stem().u8string().c_str());
            }

            ImGui::InputText("name", name, sizeof(name));

            if (ImGui::Button("OK", ImVec2(120, 0))) {
                m_Engines.emplace_back(std::pair{ name, path });
                s_SelectedEngine = m_Engines.end();

                // "Reset" the strings for next time popup is opened
                path[0] = '\0';
                name[0] = '\0';
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                // "Reset" the strings for next time popup is opened
                path[0] = '\0';
                name[0] = '\0';
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        for (auto it = m_Engines.begin(); it != m_Engines.end();) {
            const std::string& name = it->first;
            const std::filesystem::path& path = it->second;

            ImGui::Separator();

            ImGui::Text("%s", name.c_str());

            ImGui::PushID((int)std::distance(m_Engines.begin(), it));

            if (ImGui::Button("Start engine")) {
                ImGui::PopID();

                try {
                    m_RunningEngine = Engine::Create(path);
                    m_RunningEngine->SetUpdateCallback([this](const Engine::BestContinuation& c) { OnEngineUpdate(c); });
                    m_RunningEngine->Init();
                    m_RunningEngine->SetPosition(m_BoardFEN);
                    m_RunningEngine->Run();

                    s_SelectedEngine = it;
                }
                catch (EngineCreationFailure&) {
                    ImGui::OpenPopup("Failed to create engine");
                }
                catch (EngineNotReady&) {
                    ImGui::OpenPopup("Engine not ready");
                }
            }
            else {
                ImGui::PopID();
            }

            if (ImGui::Button("Delete engine")) {
                it = m_Engines.erase(it);
                s_SelectedEngine = m_Engines.end();
            }
            else {
                ++it;
            }
        }

        ImGui::Separator();
    } else {
        if (m_RunningEngine->GetThreadException() != nullptr) {
            std::rethrow_exception(m_RunningEngine->GetThreadException());
        }

        ImGui::Text("%s", s_SelectedEngine->first.c_str());

        if (ImGui::Button("Stop engine")) {
            m_RunningEngine->Stop();  // Stop the engine
            m_RunningEngine.reset();  // Stop the process
            s_SelectedEngine = m_Engines.end();
        } else {
            ImGui::Text("Depth: %i", m_BestContinuation.Depth);

            if (!m_BestContinuation.Mate) {
                float score = (float)m_BestContinuation.Score * 0.01f;
                if (m_Board.GetPlayerTurn() == Black) score *= -1.0f;

                ImGui::Text("Score: %.2f", score);
            } else {
                const char* text = (m_Board.GetPlayerTurn() == White) ? "Score: M%i" : "Score: -M%i";
                ImGui::Text(text, m_BestContinuation.Score);
            }

            ImGui::Text("%s", m_BestContinuationAlgebraicMoves.c_str());
            if (ImGui::Button("Copy to clipboard"))
                ImGui::SetClipboardText(m_BestContinuationAlgebraicMoves.c_str());
        }
    }

    // TODO: ImGui Error message wrapper?
    ImVec2 centre = ImGui::GetMainViewport()->GetCentre();
    ImGui::SetNextWindowPos(centre, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Failed to create engine")) {
        if (ImGui::Button("OK"))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    centre = ImGui::GetMainViewport()->GetCentre();
    ImGui::SetNextWindowPos(centre, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Engine not ready")) {
        if (ImGui::Button("OK"))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    ImGui::End();
}

void Application::OnEngineUpdate(const Engine::BestContinuation& bestContinuation) {
    m_BestContinuation = bestContinuation;

    Board moveTranslator(m_Board);

    std::ostringstream continuationText;
    for (LongAlgebraicMove m : m_BestContinuation.Continuation)
        continuationText << moveTranslator.Move(m) << " ";

    m_BestContinuationAlgebraicMoves = continuationText.str();
}
