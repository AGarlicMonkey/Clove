#include <Clove/FileSystem/AssetPtr.hpp>
#include <gtest/gtest.h>

using namespace clove;

class MockFile {
public:
    bool isLoaded{ false };
};

namespace {
    MockFile loadMockFile() {
        return MockFile{ true };
    }
}

TEST(AssetPtrTest, ProperlyDefaultInitialises) {
    AssetPtr<MockFile> asset{};

    EXPECT_FALSE(asset.isValid());
    EXPECT_FALSE(asset.isLoaded());
}

TEST(AssetPtrTest, CanInitialiseWithAFilePath) {
    size_t const randomHash{ 1234 };
    AssetPtr<MockFile> asset{ randomHash, &loadMockFile };

    EXPECT_TRUE(asset.isValid());
    EXPECT_FALSE(asset.isLoaded());
}

TEST(AssetPtrTest, CanGetAssetPtrHash) {
    size_t const randomHash{ 1234 };
    AssetPtr<MockFile> asset{ randomHash, &loadMockFile };

    EXPECT_EQ(asset.getHash(), randomHash);
}

TEST(AssetPtrTest, CanLoadFileWhenRequested) {
    AssetPtr<MockFile> emptyPtr{};

    EXPECT_DEATH(MockFile const &invalidFile{ emptyPtr.get() }, "");

    size_t const randomHash{ 1234 };

    AssetPtr<MockFile> asset{ randomHash, &loadMockFile };
    AssetPtr<MockFile> const constAsset{ randomHash, &loadMockFile };

    ASSERT_TRUE(asset.isValid());
    ASSERT_FALSE(asset.isLoaded());

    ASSERT_TRUE(constAsset.isValid());
    ASSERT_FALSE(constAsset.isLoaded());

    MockFile const &loadedFile{ asset.get() };
    MockFile &fileRef{ *asset };

    EXPECT_TRUE(asset.isLoaded());
    EXPECT_TRUE(asset->isLoaded);
    EXPECT_TRUE(loadedFile.isLoaded);
    EXPECT_TRUE(fileRef.isLoaded);

    MockFile const &loadedConstFile{ constAsset.get() };

    EXPECT_TRUE(constAsset.isLoaded());
    EXPECT_TRUE(loadedConstFile.isLoaded);
}

TEST(AssetPtrTest, CanPointToSameAssetButOnlyLoadOnce) {
    int32_t callCount{ 0 };
    int32_t constexpr expectedCallAmount{ 1 };

    auto const loadMockFileCount = [&callCount]() mutable {
        ++callCount;
        return MockFile{ true };
    };

    size_t const randomHash{ 1234 };

    AssetPtr<MockFile> asset{ randomHash, loadMockFileCount };
    AssetPtr<MockFile> assetCopy{ asset };

    ASSERT_TRUE(asset.isValid());
    EXPECT_EQ(asset.isValid(), assetCopy.isValid());

    MockFile const &loadedFile{ assetCopy.get() };

    ASSERT_TRUE(assetCopy.isLoaded());
    EXPECT_EQ(asset.isLoaded(), assetCopy.isLoaded());
    EXPECT_TRUE(asset->isLoaded);
    EXPECT_EQ(callCount, expectedCallAmount);

    AssetPtr<MockFile> invalidAsset{};
    AssetPtr<MockFile> invalidCopy{ invalidAsset };

    EXPECT_FALSE(invalidAsset.isValid());
    EXPECT_FALSE(invalidCopy.isValid());
}