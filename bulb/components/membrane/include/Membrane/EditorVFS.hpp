#pragma once

#include <Clove/FileSystem/VirtualFileSystem.hpp>

namespace membrane {
    /**
     * @brief VFS for the editor. As the editor stores meta data for an asset this
     * VFS resolves to the true location of the asset, not to the path of the meta data.
     */
    class EditorVFS : public clove::VirtualFileSystem {
        //VARIABLES
    private:
        std::filesystem::path contentDirectory{};

        //FUNCTIONS
    public:
        EditorVFS() = delete;
        inline EditorVFS(std::filesystem::path contentDirectory);

        EditorVFS(EditorVFS const &other) = delete;
        inline EditorVFS(EditorVFS &&other) noexcept;

        EditorVFS &operator=(EditorVFS const &other) = delete;
        inline EditorVFS &operator=(EditorVFS &&other) noexcept;

        inline ~EditorVFS();

        std::filesystem::path resolve(Path const &path) override;
    };
}

#include "EditorVFS.inl"