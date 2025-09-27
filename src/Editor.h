#pragma once
#include "DefinitionCore.hpp"

namespace v3d {
class Engine;
class Scene;
namespace editor {
class Editor {
   private:
    // Variables-----------------------------------
    Engine* m_engine;

    // Methods-------------------------------------
    Editor() {}
    Editor(Editor const&) = delete;
    Editor& operator=(Editor const&) = delete;
    ~Editor() {};

   public:
    // Methods-------------------------------------
    static Editor* Instance() {
        static Editor* instance;

        if (!instance) instance = new Editor();

        return instance;
    }

    void Init(Engine* m_engine);

    void renderGui(float deltaTime, Entity* root, Scene* scene);
    void renderHierarchyGui(Entity* entity);
    void renderEntityEditorPropertiesGui(Entity* entity);
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
