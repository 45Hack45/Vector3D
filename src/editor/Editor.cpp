

#include "editor/Editor.h"

#include <plog/Log.h>

#include <filesystem>
#include <string>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "engine.h"
#include "input/InputConfig.hpp"
#include "input/InputManager.h"
#include "scene.h"

namespace v3d {
namespace editor {

void Editor::renderHierarchyGui(entity_ptr entity) {
    std::string name = entity->getName();

    std::string itemid = "##";
    itemid += name;
    itemid += "_GUI_Selectable";

    ImGuiTreeNodeFlags flags = 0;

    if (entity->getChilds().size() <= 0)
        flags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Leaf;

    if (&entity.get() == selected) flags |= ImGuiTreeNodeFlags_Selected;

    flags |= ImGuiTreeNodeFlags_SpanAvailWidth |
             ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnArrow |
             ImGuiTreeNodeFlags_OpenOnDoubleClick;

    bool open = ImGui::TreeNodeEx(name.c_str(), flags);
    bool clicked = ImGui::IsItemClicked();

    if (clicked && !ImGui::IsItemToggledOpen()) {
        selected = &entity.get();
    }

    // if (ImGui::BeginDragDropTarget()) {
    //     if (const ImGuiPayload* payload =
    //             ImGui::AcceptDragDropPayload("HierarchyGUI_DragDrop")) {
    //         IM_ASSERT(payload->DataSize == sizeof(Entity*));
    //         Entity* p = *(Entity**)payload->Data;

    //         p->setParent(entity);
    //     }

    //     ImGui::EndDragDropTarget();
    // }

    // if (ImGui::BeginDragDropTarget()) {
    //     if (const ImGuiPayload* payload =
    //             ImGui::AcceptDragDropPayload(_DragDropModel)) {
    //         IM_ASSERT(payload->DataSize == sizeof(Model*));
    //         Model* p = *(Model**)payload->Data;

    //         engine->scene->loadModel2Scene(p);
    //     }

    //     ImGui::EndDragDropTarget();
    // }

    // if (ImGui::BeginDragDropSource()) {
    //     ImGui::SetDragDropPayload("HierarchyGUI_DragDrop", &entity,
    //                               sizeof(Entity**));
    //     ImGui::Text(entity->m_name.c_str());
    //     ImGui::EndDragDropSource();
    // }

    // // if (ImGui::IsMouseDragging(ImGuiMouseButton_::ImGuiMouseButton_Left))
    // {
    // //	std::string temp = "##PlaceHolder_";
    // //	temp += entity->name;
    // //	ImGui::Selectable(temp.c_str(), false, 0, ImVec2(0, 5));
    // //	if (ImGui::BeginDragDropTarget()) {
    // //		if (const ImGuiPayload* payload =
    // // ImGui::AcceptDragDropPayload("HierarchyGUI_DragDrop")) {

    // //		}
    // //		ImGui::EndDragDropTarget();
    // //	}
    // //}

    if (open) {
        for (auto child : entity->getChilds()) {
            renderHierarchyGui(child);
        }

        ImGui::TreePop();
    }
}

namespace {
const ImVec4 kOkColor(0.4f, 0.9f, 0.4f, 1.0f);
const ImVec4 kWarnColor(1.0f, 0.75f, 0.2f, 1.0f);
const ImVec4 kErrorColor(1.0f, 0.4f, 0.4f, 1.0f);

// Unresolved bindings are retained in the config, so the panel must show them.
std::size_t countUnresolved(const input::DeviceProfile& profile) {
    std::size_t unresolved = 0;
    for (const input::KeyBinding& binding : profile.bindings) {
        if (input::bindingStatus(binding) != input::BindingStatus::Resolved)
            unresolved++;
    }
    return unresolved;
}

std::size_t countUnresolved(const input::DeviceConfig& device) {
    std::size_t unresolved = 0;
    for (const input::DeviceProfile& profile : device.profiles) {
        unresolved += countUnresolved(profile);
    }
    return unresolved;
}

// Ddeferred profile edits.
struct PendingProfileAction {
    enum class Kind { None, Activate, Duplicate, Remove };

    Kind kind = Kind::None;
    std::string guid;
    std::string profile;
};

const char* bindingStatusName(input::BindingStatus status) {
    switch (status) {
        case input::BindingStatus::UnknownAction:
            return "unknown action";
        case input::BindingStatus::UnknownKey:
            return "unknown key";
        case input::BindingStatus::Resolved:
            break;
    }
    return "resolved";
}
}  // namespace

void Editor::renderInputConfigGui() {
    InputManager* inputManager = m_engine->getInputManager();
    const std::filesystem::path configPath = InputManager::defaultConfigPath();

    ImGui::TextUnformatted("Config file");
    ImGui::TextWrapped("%s", configPath.string().c_str());

    if (inputManager->isConfigDirty()) {
        ImGui::TextColored(kWarnColor, "Modified - not saved");
    } else {
        ImGui::TextColored(kOkColor, "Saved");
    }

    ImGui::Spacing();

    if (ImGui::Button("Save input config")) {
        const input::InputConfigResult result = inputManager->saveConfig(configPath);
        m_lastInputConfigOk = result.ok;
        m_lastInputConfigMessage = result.message;
        if (result.ok) {
            PLOGI << "Input config: " << result.message;
        } else {
            PLOGE << "Input config: " << result.message;
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Load input config")) {
        const input::InputConfigResult result = inputManager->loadConfig(configPath);
        m_lastInputConfigOk = result.ok;
        m_lastInputConfigMessage = result.message;
        if (result.ok) {
            PLOGI << "Input config: " << result.message;
        } else {
            PLOGE << "Input config: " << result.message;
        }
    }

    if (!m_lastInputConfigMessage.empty()) {
        ImGui::TextColored(m_lastInputConfigOk ? kOkColor : kErrorColor, "%s",
                           m_lastInputConfigMessage.c_str());
    }

    ImGui::Spacing();
    ImGui::SeparatorText("Connected devices");

    const ImGuiTableFlags tableFlags = ImGuiTableFlags_Borders |
                                       ImGuiTableFlags_RowBg |
                                       ImGuiTableFlags_SizingStretchProp;

    PendingProfileAction pending;

    if (ImGui::BeginTable("##inputConnectedDevices", 4, tableFlags)) {
        ImGui::TableSetupColumn("Kind");
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("GUID");
        ImGui::TableSetupColumn("Active profile");
        ImGui::TableHeadersRow();

        for (std::size_t i = 0; i < inputManager->getNumDevices(); i++) {
            input::InputDevice* device =
                inputManager->getDevice(static_cast<uint8_t>(i));
            if (!device) continue;

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(input::deviceKindName(device->getDeviceType()));
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(device->getName().c_str());
            ImGui::TableNextColumn();
            ImGui::TextUnformatted(device->getGuid().c_str());
            ImGui::TableNextColumn();

            const input::DeviceConfig* config =
                inputManager->configStore().findDevice(device->getGuid());
            if (!config) {
                ImGui::TextColored(kWarnColor, "no saved profile");
                continue;
            }

            const input::DeviceProfile* active = config->resolvedProfile();
            const std::string comboId = "##activeProfile_" + config->guid;

            ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::BeginCombo(comboId.c_str(),
                                  active ? active->name.c_str() : "(none)")) {
                for (const input::DeviceProfile& profile : config->profiles) {
                    const bool selected = active && &profile == active;
                    if (ImGui::Selectable(profile.name.c_str(), selected)) {
                        pending = {PendingProfileAction::Kind::Activate,
                                   config->guid, profile.name};
                    }
                    if (selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            const std::size_t unresolved = countUnresolved(*config);
            if (unresolved > 0) {
                ImGui::TextColored(kWarnColor, "%zu unresolved binding(s)",
                                   unresolved);
            }
        }
        ImGui::EndTable();
    }

    ImGui::Spacing();
    ImGui::SeparatorText("Saved profiles");

    const auto& devices = inputManager->configStore().devices();
    if (devices.empty()) {
        ImGui::TextDisabled("No profiles stored");
        return;
    }

    for (const input::DeviceConfig& config : devices) {
        const bool connected = inputManager->findDeviceByGuid(config.guid) != nullptr;

        std::string deviceLabel = config.lastKnownName;
        deviceLabel += " (";
        deviceLabel += config.deviceKind;
        deviceLabel += ")##device_";
        deviceLabel += config.guid;

        if (!ImGui::TreeNode(deviceLabel.c_str())) continue;

        ImGui::Text("GUID: %s", config.guid.c_str());
        if (connected) {
            ImGui::TextColored(kOkColor, "Connected");
        } else {
            ImGui::TextColored(kWarnColor, "Retained (device not connected)");
        }

        const input::DeviceProfile* active = config.resolvedProfile();

        for (const input::DeviceProfile& profile : config.profiles) {
            const bool isActive = active && &profile == active;

            std::string profileLabel = profile.name;
            if (isActive) profileLabel += "  [active]";
            profileLabel += "##profile_";
            profileLabel += config.guid;
            profileLabel += "_";
            profileLabel += profile.name;

            if (!ImGui::TreeNode(profileLabel.c_str())) continue;

            if (!isActive && ImGui::Button("Activate")) {
                pending = {PendingProfileAction::Kind::Activate, config.guid,
                           profile.name};
            }
            if (!isActive) ImGui::SameLine();
            if (ImGui::Button("Duplicate")) {
                pending = {PendingProfileAction::Kind::Duplicate, config.guid,
                           profile.name};
            }
            ImGui::SameLine();
            ImGui::BeginDisabled(config.profiles.size() <= 1);
            if (ImGui::Button("Remove")) {
                pending = {PendingProfileAction::Kind::Remove, config.guid,
                           profile.name};
            }
            ImGui::EndDisabled();

            const std::string bindingsId = "##bindings_" + config.guid + profile.name;
            if (ImGui::BeginTable(bindingsId.c_str(), 3, tableFlags)) {
                ImGui::TableSetupColumn("Action");
                ImGui::TableSetupColumn("Key");
                ImGui::TableSetupColumn("Status");
                ImGui::TableHeadersRow();

                for (const input::KeyBinding& binding : profile.bindings) {
                    const input::BindingStatus status =
                        input::bindingStatus(binding);

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(binding.action.c_str());
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(binding.key.c_str());
                    ImGui::TableNextColumn();
                    if (status == input::BindingStatus::Resolved) {
                        ImGui::TextUnformatted(bindingStatusName(status));
                    } else {
                        ImGui::TextColored(kWarnColor, "%s",
                                           bindingStatusName(status));
                    }
                }
                ImGui::EndTable();
            }

            ImGui::TreePop();
        }

        ImGui::TreePop();
    }

    switch (pending.kind) {
        case PendingProfileAction::Kind::Activate:
            inputManager->setActiveProfile(pending.guid, pending.profile);
            break;
        case PendingProfileAction::Kind::Duplicate:
            inputManager->duplicateProfile(pending.guid, pending.profile,
                                           pending.profile + " copy");
            break;
        case PendingProfileAction::Kind::Remove:
            inputManager->removeProfile(pending.guid, pending.profile);
            break;
        case PendingProfileAction::Kind::None:
            break;
    }
}

void Editor::renderGui(float deltaTime, entity_ptr root, Scene* scene) {
    //-----------------------------Render
    // GUI------------------------------------------

    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    // Main Menu
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::Button("Delete")) {
                PLOGW << "Not implemented" << std::endl;
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Asset")) {
            if (ImGui::BeginMenu("Create")) {
                if (ImGui::Button("Material")) {
                    PLOGW << "Not implemented" << std::endl;
                }
                ImGui::Spacing();
                if (ImGui::Button("Physics Material")) {
                    PLOGW << "Not implemented" << std::endl;
                }
                ImGui::EndMenu();
            }

            ImGui::Spacing();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Scene")) {
            if (ImGui::BeginMenu("Entity")) {
                if (ImGui::Button("Create Entity")) {
                    entity_ptr entity = scene->instantiateEntity("Entity");
                    selected = &entity.get();
                }
                if (ImGui::Button("Create Entity From Model")) {
                    PLOGW << "Not implemented" << std::endl;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    ImGui::Begin("Frame Rate", NULL,
                 ImGuiWindowFlags_AlwaysAutoResize |
                     ImGuiWindowFlags_NoBackground |
                     ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / io.Framerate, io.Framerate);
    ImGui::End();

    ImGuiWindowFlags windowsFlags =
        ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoCollapse | 0 |
        0;  // ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove

    // // Resources
    // if (ImGui::Begin("Resources", NULL, windowsFlags)) {
    //     renderAssetsGui();
    // }
    // ImGui::End();

    // hierarchy
    if (ImGui::Begin("Scene", NULL, windowsFlags)) {
        // ImVec2 size = ImGui::GetWindowSize();
        ////Dummy
        // ImGui::SameLine();
        // ImGui::Dummy(ImVec2(0,0));

        // if (ImGui::BeginDragDropTarget()) {
        //	if (const ImGuiPayload* payload =
        // ImGui::AcceptDragDropPayload(_DragDropModel)) {
        //		IM_ASSERT(payload->DataSize == sizeof(Model*));
        //		Model* m = *(Model**)payload->Data;
        //		scene->loadModel2Scene(m);
        //	}

        //	ImGui::EndDragDropTarget();
        //}

        // ImGui::SameLine();

        // renderHierarchyGui(&scene->m_scene);

        for (auto entity : root->getChilds()) renderHierarchyGui(entity);
    }
    ImGui::End();

    // Properties
    if (ImGui::Begin("Properties", NULL, windowsFlags)) {
        if (selected) selected->drawEditorGUI_Properties();
    }
    ImGui::End();

    // Input
    if (ImGui::Begin("Input", NULL, windowsFlags)) {
        renderInputConfigGui();
    }
    ImGui::End();

    if (showImGui_Demo) ImGui::ShowDemoWindow(&showImGui_Demo);
}

}  // namespace editor

}  // namespace v3d
