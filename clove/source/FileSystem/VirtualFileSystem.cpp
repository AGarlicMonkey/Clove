#include "Clove/FileSystem/VirtualFileSystem.hpp"

#include <Clove/Log/Log.hpp>

namespace clove {
    VirtualFileSystem::VirtualFileSystem() = default;

    VirtualFileSystem::VirtualFileSystem(VirtualFileSystem &&other) noexcept = default;

    VirtualFileSystem &VirtualFileSystem::operator=(VirtualFileSystem &&other) noexcept = default;

    VirtualFileSystem::~VirtualFileSystem() = default;

    void VirtualFileSystem::mount(std::filesystem::path systemPath, std::string const &alias) {
        mountedPaths[alias] = systemPath.make_preferred();
    }

    std::filesystem::path VirtualFileSystem::resolve(std::filesystem::path const &vfsPath) {
        std::string const vfsRoot{ vfsPath.begin()->string() };
        std::filesystem::path fullPath{};

        if(mountedPaths.contains(vfsRoot)) {
            fullPath = mountedPaths.at(vfsRoot);
            auto const relPath{ vfsPath.lexically_relative(vfsRoot) };
            if(!relPath.empty() && relPath.string() != ".") {
                fullPath /= relPath;
            }
        } else {
            CLOVE_LOG(LOG_CATEGORY_CLOVE, LogLevel::Error, "{0}: Unknown VFS root: \"{1}\". Returning empty path", CLOVE_FUNCTION_NAME, vfsRoot);
        }

        return fullPath.make_preferred();
    }
}