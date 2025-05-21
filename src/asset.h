#pragma once

#ifndef ASSET_H
#define ASSET_H

// #include "logger.h"
#include <functional>  //required for the hash method
#include <string>

// #include "IEditorGUISelectable.h"

#include <plog/Log.h>

#define DisplayedNameMaxSize 64

namespace v3d {
class Asset  //: public IEditorGUISelectable
{
   public:
    Asset(const std::string& file, const std::string& name) {
        filePath = file;
        assetName = name;
    }

    // virtual void drawEditorGUI_Properties();

    std::string assetName;
    std::string filePath;

    virtual void loadFile() {
        PLOGE << "ERROR::ASSET::CALLED_BASE_CLASS_METHOD:: void loadFile\n";
    }

    size_t hash() {
        size_t Hpath = std::hash<std::string>{}(filePath);
        size_t HName = std::hash<std::string>{}(assetName);

        return Hpath ^ (HName << 1);
    }

    std::string fileDirectory() {
        size_t separatorIndex = filePath.find_last_of('/');

        if (separatorIndex == std::string::npos) {
            PLOGW << "ERROR::ASSET::FILE_DIRECTORY_NOT_FOUND " << filePath << "\n";
            return "";
        }

        return filePath.substr(0, separatorIndex);
    }
    std::string fileName() {
        size_t separatorIndex = filePath.find_last_of('/') + 1;
        size_t extensionIndex = filePath.find_last_of('.');

        if (separatorIndex == std::string::npos)
            separatorIndex = 0;

        if (extensionIndex == std::string::npos)
            extensionIndex = (size_t)filePath.size();

        return filePath.substr(separatorIndex, extensionIndex - separatorIndex);
    }
    std::string fileExtension() {
        size_t separatorIndex = filePath.find_last_of('.');

        if (separatorIndex == std::string::npos) {
            PLOGW << "ERROR::ASSET::FILE_EXTENSION_NOT_FOUND " << filePath << "\n";
            return "";
        }

        return filePath.substr(separatorIndex, filePath.size());
    }
    std::string fileNameAndExtension() {
        size_t separatorIndex = filePath.find_last_of('/') + 1;

        if (separatorIndex == std::string::npos) {
            PLOGW << "ERROR::ASSET::FILE_DIRECTORY_NOT_FOUND " << filePath << "\n";
            return "";
        }

        return filePath.substr(separatorIndex, filePath.size() - separatorIndex);
    }
};

}  // namespace v3d

#endif
