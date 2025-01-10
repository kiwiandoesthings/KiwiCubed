#pragma once
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_sdl2.h>

#include "SingleplayerHandler.h"
#include "Window.h"
#include "klogger.hpp"

class DataPanel {
  public:
    SingleplayerHandler *sph = nullptr;
    Window *globalWindow = nullptr;

    DataPanel(SingleplayerHandler *sph, Window *globalWindow, const char *glsl_version) : sph(sph), globalWindow(globalWindow) {
        Setup(glsl_version);
    }

    void addPointers(SingleplayerHandler *sph, Window *globalWindow) {
        this->sph = sph;
        this->globalWindow = globalWindow;
    }

    void Setup(const char *glsl_version) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        io.IniFilename = NULL; // don't save the accursed imgui.ini
        ImGui_ImplSDL2_InitForOpenGL(globalWindow->GetWindowInstance(), globalWindow->gl_context);
        ImGui_ImplOpenGL3_Init(glsl_version);
    }

    void Draw(int frames, double fps, EntityData playerData) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Debug");
        if (ImGui::CollapsingHeader("Player Info")) {
            ImGui::Text("Player name: %s", sph->singleplayerWorld.player.GetEntityData().name);
            ImGui::Text("Player health: %d", static_cast<int>(sph->singleplayerWorld.player.GetEntityStats().health));
            ImGui::Text("Player position: %f, %f, %f", playerData.position.x, playerData.position.y, playerData.position.z);
            ImGui::Text("Player orientation: %f, %f, %f", playerData.orientation.x, playerData.orientation.y, playerData.orientation.z);
            ImGui::Text(
                "Player velocity: %f, %f, %f", sph->singleplayerWorld.player.GetEntityData().velocity.x,
                sph->singleplayerWorld.player.GetEntityData().velocity.y, sph->singleplayerWorld.player.GetEntityData().velocity.z
            );
            ImGui::Text(
                "Global chunk position: %d, %d, %d", static_cast<int>(sph->singleplayerWorld.player.GetEntityData().globalChunkPosition.x),
                static_cast<int>(sph->singleplayerWorld.player.GetEntityData().globalChunkPosition.y),
                static_cast<int>(sph->singleplayerWorld.player.GetEntityData().globalChunkPosition.z)
            );
            ImGui::Text(
                "Local chunk position: %d, %d, %d", static_cast<int>(sph->singleplayerWorld.player.GetEntityData().localChunkPosition.x),
                static_cast<int>(sph->singleplayerWorld.player.GetEntityData().localChunkPosition.y),
                static_cast<int>(sph->singleplayerWorld.player.GetEntityData().localChunkPosition.z)
            );
            ImGui::Text(
                "Current chunk generation status and blocks %d, %d, %d",
                sph->singleplayerWorld
                    .GetChunk(
                        sph->singleplayerWorld.player.GetEntityData().globalChunkPosition.x,
                        sph->singleplayerWorld.player.GetEntityData().globalChunkPosition.y,
                        sph->singleplayerWorld.player.GetEntityData().globalChunkPosition.z
                    )
                    .generationStatus,
                sph->singleplayerWorld
                    .GetChunk(
                        sph->singleplayerWorld.player.GetEntityData().globalChunkPosition.x,
                        sph->singleplayerWorld.player.GetEntityData().globalChunkPosition.y,
                        sph->singleplayerWorld.player.GetEntityData().globalChunkPosition.z
                    )
                    .GetTotalBlocks(),
                sph->singleplayerWorld
                    .GetChunk(
                        sph->singleplayerWorld.player.GetEntityData().globalChunkPosition.x,
                        sph->singleplayerWorld.player.GetEntityData().globalChunkPosition.y,
                        sph->singleplayerWorld.player.GetEntityData().globalChunkPosition.z
                    )
                    .id
            );
            ImGui::Text("Total frames: %d", frames);
            ImGui::Text("FPS: %.2f", fps);
        }

        if (ImGui::CollapsingHeader("World")) {
            sph->singleplayerWorld.DisplayImGui(0);
        }

        if (ImGui::CollapsingHeader("Chunk Info")) {
            sph->singleplayerWorld.DisplayImGui(1);
        }
        ImGui::End();
    }

    void SwapFrames() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Destroy() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }
};
