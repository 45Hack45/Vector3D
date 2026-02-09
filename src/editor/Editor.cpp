

#include "editor/Editor.h"

#include <plog/Log.h>

#include <string>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "scene.h"

namespace v3d {
namespace editor {

void Editor::renderHierarchyGui(Entity* entity) {
    std::string name = entity->getName();

    std::string itemid = "##";
    itemid += name;
    itemid += "_GUI_Selectable";

    ImGuiTreeNodeFlags flags = 0;

    if (entity->getChilds().size() <= 0)
        flags |= ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Leaf;

    if (entity == selected) flags |= ImGuiTreeNodeFlags_Selected;

    flags |= ImGuiTreeNodeFlags_SpanAvailWidth |
             ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnArrow |
             ImGuiTreeNodeFlags_OpenOnDoubleClick;

    bool open = ImGui::TreeNodeEx(name.c_str(), flags);
    bool clicked = ImGui::IsItemClicked();

    if (clicked && !ImGui::IsItemToggledOpen()) {
        selected = entity;
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
            renderHierarchyGui(&child.get());
        }

        ImGui::TreePop();
    }
}

void Editor::renderGui(float deltaTime, Entity* root, Scene* scene) {
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
                    scene->instantiateEntity("Entity");
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

        for (auto entity : root->getChilds()) renderHierarchyGui(&entity.get());
    }
    ImGui::End();

    // Properties
    if (ImGui::Begin("Properties", NULL, windowsFlags)) {
        if (selected) selected->drawEditorGUI_Properties();
    }
    ImGui::End();

    if (showImGui_Demo) ImGui::ShowDemoWindow(&showImGui_Demo);
}

}  // namespace editor

}  // namespace v3d
