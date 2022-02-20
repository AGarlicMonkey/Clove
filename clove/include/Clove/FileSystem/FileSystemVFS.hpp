#pragma once

#include "Clove/FileSystem/VirtualFileSystem.hpp"

namespace clove {
    /**
     * @brief VFS implementation that points directly to lose files on the local platform.
     */
    class FileSystemVFS : public VirtualFileSystem {
        //VARIABLES
    private:
        std::unordered_map<std::string, std::filesystem::path> mountedPaths{};

        //FUNCTIONS
    public:
        inline FileSystemVFS();

        FileSystemVFS(FileSystemVFS const &other) = delete;
        inline FileSystemVFS(FileSystemVFS &&other) noexcept;

        FileSystemVFS &operator=(FileSystemVFS const &other) = delete;
        inline FileSystemVFS &operator=(FileSystemVFS &&other) noexcept;

        inline ~FileSystemVFS();

        /**
         * @brief Mounts a system path to an alias.
         * @details Note that system paths are mounted as: std::filesystem::path::make_prefered
         * @param systemPath System path to mount.
         * @param alias The alias to give the full system path.
         */
        void mount(std::filesystem::path systemPath, std::string const &alias);

        std::filesystem::path resolve(std::filesystem::path const &vfsPath) override;
    };
}

#include "FileSystemVFS.inl"