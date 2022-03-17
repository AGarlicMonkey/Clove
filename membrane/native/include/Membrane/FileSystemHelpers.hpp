#pragma once

#include "Membrane/Export.hpp"

#include <Clove/Guid.hpp>
#include <string>
#include <wtypes.h>

MEMBRANE_EXPORT BSTR getContentPath();

MEMBRANE_EXPORT bool isFileSupported(std::string file);

MEMBRANE_EXPORT bool isAssetFile(std::string file);

MEMBRANE_EXPORT int32_t getFileType(std::string file);

MEMBRANE_EXPORT void createAssetFile(std::string assetLocation, std::string fileToCreateFrom, std::string relPathOfCreateFrom, std::string assetVfsPath);
MEMBRANE_EXPORT void moveAssetFile(std::string sourceFileName, std::string destFileName);
MEMBRANE_EXPORT void removeAssetFile(clove::Guid::Type assetGuid, int32_t assetFileType);

MEMBRANE_EXPORT int32_t getAssetFileType(std::string fullFilePath);
MEMBRANE_EXPORT clove::Guid::Type getAssetFileGuid(std::string fullFilePath);
