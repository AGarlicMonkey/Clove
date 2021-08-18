#include <Clove/FileSystem/VirtualFileSystem.hpp>
#include <gtest/gtest.h>

using namespace clove;

TEST(VirtualFileSystemTest, CanMountAndResolveAFilePath) {
    std::filesystem::path const rootPath{ "C:/Users/Test/Some/Root/Path" };
    std::filesystem::path const mountPath{ "C:/Users/Test/Some/Full/Path" };
    std::filesystem::path const emptyPath{};

    VirtualFileSystem vfs{};

    vfs.mount(rootPath, ".");
    vfs.mount(mountPath, "saves");

    EXPECT_EQ(vfs.resolve("."), rootPath);
    EXPECT_EQ(vfs.resolve("saves"), mountPath);
    EXPECT_EQ(vfs.resolve("notMounted"), emptyPath);
}

TEST(VirtualFileSystemTest, CanResolveNestedPaths) {
    std::filesystem::path const rootPath{ "C:/Users/Test/Some/Root/Path" };
    std::filesystem::path const mountPath{ "C:/Users/Test/Some/Full/Path" };

    VirtualFileSystem vfs{};

    vfs.mount(rootPath, ".");
    vfs.mount(mountPath, "data");

    EXPECT_EQ(vfs.resolve("./saves"), rootPath / "saves");
    EXPECT_EQ(vfs.resolve("data/models"), mountPath / "models");
    EXPECT_EQ(vfs.resolve("data/models/cube.obj"), mountPath / "models/cube.obj");
}

TEST(VirtualFileSystemTest, CanMountNestedPaths) {
    std::filesystem::path const rootPath{ "C:/Users/Test/Some/Root/Path" };
    std::filesystem::path const mountPath{ rootPath / "Generated" };

    VirtualFileSystem vfs{};

    vfs.mount(rootPath, ".");
    vfs.mount(mountPath, "generated");
    vfs.mount(rootPath / "assets", "assets");

    EXPECT_EQ(vfs.resolve("./Generated"), vfs.resolve("generated"));
    EXPECT_EQ(vfs.resolve("./assets"), vfs.resolve("assets"));
}