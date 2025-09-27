

#include "Editor.h"

#include <string>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "scene.h"

#define _nameMaxSize 64

namespace v3d {
namespace editor {
void Editor::Init(Engine* engine) { m_engine = engine; }

void Editor::renderHierarchyGui(Entity* entity) {
    std::string name = entity->m_name;

    std::string itemid = "##";
    itemid += entity->m_name;
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

void Editor::renderEntityEditorPropertiesGui(Entity* entity) {
    if (!entity) return;

    char* name = new char[_nameMaxSize];

    name = (char*)entity->m_name.c_str();

    ImGui::InputText("Entity", name, _nameMaxSize);
    entity->m_name = name;

    // ImGui::Text(entity->m_name.c_str());
    ImGui::Spacing();

    // // Draw Transform
    // if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
    // {
    //     entity->transform.drawEditorGUI_Properties();
    // }

    // ImGui::Spacing();
    // ImGui::Spacing();

    // // Draw meshrenderer
    // if (ImGui::CollapsingHeader("MeshRenderer"))
    //     entity->meshRenderer.drawEditorGUI_Properties();

    // ImGui::Spacing();
    // ImGui::Spacing();

    // // Draw material
    // if (entity->meshRenderer.m_material)
    //     if (ImGui::CollapsingHeader("Material",
    //     ImGuiTreeNodeFlags_DefaultOpen))
    //         entity->meshRenderer.m_material->drawEditorGUI_Properties();

    // ImGui::Spacing();
    // ImGui::Spacing();

    // Draw components
    for (auto component : entity->getComponents()) {
        if (ImGui::CollapsingHeader(component->getComponentName().c_str())) {
            component->drawEditorGUI_Properties();
        }
        ImGui::Spacing();
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

        if (ImGui::BeginMenu("Entity")) {
            if (ImGui::Button("Create Entity")) {
                scene->instantiateEntity("Entity");
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
