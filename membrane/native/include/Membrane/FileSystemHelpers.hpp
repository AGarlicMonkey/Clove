#pragma once

#include "Membrane/Export.hpp"

#include <Clove/Guid.hpp>
#include <string>
#include <wtypes.h>

MEMBRANE_EXPORT BSTR getContentPath();

MEMBRANE_EXPORT bool isFileSupported(wchar_t const *file);

MEMBRANE_EXPORT bool isAssetFile(wchar_t const *file);

MEMBRANE_EXPORT int32_t getFileType(wchar_t const *file);

MEMBRANE_EXPORT void createAssetFile(wchar_t const *assetLocation, wchar_t const *fileToCreateFrom, wchar_t const *relPathOfCreateFrom, wchar_t const *assetVfsPath);
MEMBRANE_EXPORT void moveAssetFile(wchar_t const *sourceFileName, wchar_t const *destFileName);
MEMBRANE_EXPORT void removeAssetFile(clove::Guid::Type assetGuid, int32_t assetFileType);

MEMBRANE_EXPORT int32_t getAssetFileType(wchar_t const *fullFilePath);
MEMBRANE_EXPORT clove::Guid::Type getAssetFileGuid(wchar_t const *fullFilePath);
