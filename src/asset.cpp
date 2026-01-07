#include "asset.h"

#include "editor/Editor.h"

namespace v3d {
void Asset::drawEditorGUI_Properties() {
    char* tName = new char[DisplayedNameMaxSize];

    tName = (char*)assetName.c_str();

    ImGui::InputText("Asset", tName, DisplayedNameMaxSize);
    assetName = tName;
}
}  // namespace v3d
