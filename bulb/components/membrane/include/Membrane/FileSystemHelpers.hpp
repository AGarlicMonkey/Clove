#pragma once

namespace membrane {
    public enum class FileType{
        Unknown,
        Mesh,
        Texture,
    };

    public ref class FileSystemHelpers{
        //FUNCTIONS
    public:
        static System::String ^getAssetExtension();

        /**
         * @brief Checks if the file system can support the file.
         */
        static bool isFileSupported(System::String ^file);

        /**
         * @brief Checks if the file is a *.clvasset file.
         */
        static bool isAssetFile(System::String ^ file);

        /**
         * @brief Gets the type of a file.
         */
        static FileType getFileType(System::String ^ fullFilePath);

        /**
         * @brief Creates a new .clvasset file on disk.
         */
        static void createAssetFile(System::String ^assetLocation, System::String ^fileToCreateFrom, System::String ^relPathOfCreateFrom, System::String ^assetVfsPath);
        /**
         * @brief Notifies the asset manager that a file was moved. Note: Does not move the file on disk.
         */
        static void moveAssetFile(System::String ^ sourceFileName, System::String ^ destFileName);
        /**
         * @brief Notifies the asset manager that a file was deleted. Note: Does not delete the file on disk.
         */
        static void removeAssetFile(System::UInt64 assetGuid, FileType assetFileType);

        /**
         * @brief Gets the type of an asset file (*.clvasset)
         */
        static FileType getAssetFileType(System::String ^ fullFilePath);

        static System::UInt64 getAssetFileGuid(System::String ^ fullFilePath);
    };
}