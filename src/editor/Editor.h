#pragma once
#include "DefinitionCore.hpp"

namespace v3d {
class Engine;
class Scene;
namespace editor {
#define _nameMaxSize 64;
constexpr std::string_view EditorPopup_PropertiesAddComponent =
    "editorPropretiesAddComponent";

class Editor {
    friend class Engine;

   private:
    // Variables-----------------------------------
    Engine* m_engine;

    // Methods-------------------------------------
    Editor(Editor const&) = delete;
    Editor& operator=(Editor const&) = delete;

   public:
    // Methods-------------------------------------

    Editor(Engine* engine) : m_engine(engine) {};
    ~Editor() {};

    void renderGui(float deltaTime, Entity* root, Scene* scene);
    void renderHierarchyGui(Entity* entity);
    void renderEntityEditorPropertiesGui(Entity* entity, Scene* scene);
    // void renderAssetsGui();

    // static Mesh* GUI_PropertySelector(const char* propName, Mesh*
    // currentValue); static Material* GUI_PropertySelector(const char*
    // propName,
    //                                       Material* currentValue);
    // static Shader* GUI_PropertySelector(const char* propName,
    //                                     Shader* currentValue);
    // static Texture_Asset* GUI_PropertySelector(const char* propName,
    //                                            Texture_Asset* currentValue);

    // Variables-----------------------------------
    bool showImGui_Demo = false;
    IEditorGUISelectable* selected = nullptr;
};
}  // namespace editor

}  // namespace v3d
