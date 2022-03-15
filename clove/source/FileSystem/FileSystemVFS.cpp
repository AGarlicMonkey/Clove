#include "Clove/FileSystem/FileSystemVFS.hpp"

#include <Clove/Log/Log.hpp>

CLOVE_DECLARE_LOG_CATEGORY(CloveFileSystem)

namespace clove {
    void FileSystemVFS::mount(std::filesystem::path systemPath, std::string const &alias) {
        mountedPaths[alias] = systemPath.make_preferred();
    }

    std::filesystem::path FileSystemVFS::resolve(Path const &path) {
        std::string const vfsRoot{ path.begin()->string() };
        std::filesystem::path fullPath{};

        if(mountedPaths.contains(vfsRoot)) {
            fullPath = mountedPaths.at(vfsRoot);
            auto const relPath{ path.lexically_relative(vfsRoot) };
            if(!relPath.empty() && relPath.string() != ".") {
                fullPath /= relPath;
            }
        } else {
            CLOVE_LOG(CloveFileSystem, LogLevel::Error, "{0}: Unknown VFS path: \"{1}\". Returning empty path", CLOVE_FUNCTION_NAME, vfsRoot);
        }

        return fullPath.make_preferred();
    }
}