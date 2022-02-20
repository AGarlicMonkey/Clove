#pragma once

#include <filesystem>
#include <unordered_map>

namespace clove {
    /**
     * @brief Manages a file system that mounts system paths to aliases.
     * @details These aliases will not change between systems. Providing a way
     * to keep asset locations consistent on any platform. This also allows
     * BLOBs or archives to be mounted under an alias as well.
     */
    class VirtualFileSystem {
        //FUNCTIONS
    public:
        virtual ~VirtualFileSystem() = default;

        /**
         * @brief Resolves a vfs path into a system path.
         * @details For example mounting 'data' as 'C:/files/data' then resolving
         * 'data/model.obj' will return 'C:/files/data/model.obj'.
         * @param vfsPath A path inside the VFS. THe path should begin with a mounted alias.
         * @return The resolved system path.
         */
        virtual std::filesystem::path resolve(std::filesystem::path const &vfsPath) = 0;
    };
}