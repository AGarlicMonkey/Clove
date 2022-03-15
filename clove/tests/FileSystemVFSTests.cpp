#include <Clove/FileSystem/FileSystemVFS.hpp>
#include <gtest/gtest.h>

using namespace clove;

TEST(FileSystemVFSTest, CanMountAndResolveAFilePath) {
    std::filesystem::path const rootPath{ "C:/Users/Test/Some/Root/Path" };
    std::filesystem::path const mountPath{ "C:/Users/Test/Some/Full/Path" };
    std::filesystem::path const emptyPath{};

    FileSystemVFS vfs{};

    vfs.mount(rootPath, ".");
    vfs.mount(mountPath, "saves");

    EXPECT_EQ(vfs.resolve("."), rootPath);
    EXPECT_EQ(vfs.resolve("saves"), mountPath);
    EXPECT_EQ(vfs.resolve("notMounted"), emptyPath);
}

TEST(FileSystemVFSTest, CanResolveNestedPaths) {
    std::filesystem::path const rootPath{ "C:/Users/Test/Some/Root/Path" };
    std::filesystem::path const mountPath{ "C:/Users/Test/Some/Full/Path" };

    FileSystemVFS vfs{};

    vfs.mount(rootPath, ".");
    vfs.mount(mountPath, "data");

    EXPECT_EQ(vfs.resolve("./saves"), rootPath / "saves");
    EXPECT_EQ(vfs.resolve("data/models"), mountPath / "models");
    EXPECT_EQ(vfs.resolve("data/models/cube.obj"), mountPath / "models/cube.obj");
}

TEST(FileSystemVFSTest, CanMountNestedPaths) {
    std::filesystem::path const rootPath{ "C:/Users/Test/Some/Root/Path" };
    std::filesystem::path const mountPath{ rootPath / "Generated" };

    FileSystemVFS vfs{};

    vfs.mount(rootPath, ".");
    vfs.mount(mountPath, "generated");
    vfs.mount(rootPath / "assets", "assets");

    EXPECT_EQ(vfs.resolve("./Generated"), vfs.resolve("generated"));
    EXPECT_EQ(vfs.resolve("./assets"), vfs.resolve("assets"));
}