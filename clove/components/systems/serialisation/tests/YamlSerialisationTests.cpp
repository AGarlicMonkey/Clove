#include <Clove/Serialisation/Node.hpp>
#include <Clove/Serialisation/Yaml.hpp>
#include <gtest/gtest.h>

using namespace clove;
using namespace clove::serialiser;

TEST(YamlSerialisationTests, CanEmittDefaultInformation) {
    Node node{};

    EXPECT_EQ(emittYaml(node), "type: yaml\nversion: 1");
}

TEST(YamlSerialisationTests, CanEmittAChildNode) {
    Node root{};

    root["Start"] = 0;

    EXPECT_EQ(emittYaml(root), "type: yaml\nversion: 1\nStart: 0");

    root["Node"] = 0;

    EXPECT_EQ(emittYaml(root), "type: yaml\nversion: 1\nStart: 0\nNode: 0");
}

TEST(YamlSerialisationTests, CanEmittAChildNodeWithAValue) {
    Node root{};

    float f{ 99.0f };
    int32_t i{ 100 };
    uint32_t ui{ 200 };
    int16_t i16{ 16 };
    uint16_t ui16{ 1600 };
    std::string str{ "Hello" };
    std::string_view strv{ "there" };

    root["value1"]   = 5.0f;
    root["value2"]   = 3;
    root["float"]    = f;
    root["int32_t"]  = i;
    root["uint32_t"] = ui;
    root["int16_t"]  = i16;
    root["uint16_t"] = ui16;
    root["string1"]  = str;
    root["string2"]  = strv;
    root["string3"]  = "world!";

    EXPECT_EQ(emittYaml(root), "type: yaml\nversion: 1\nvalue1: 5\nvalue2: 3\nfloat: 99\nint32_t: 100\nuint32_t: 200\nint16_t: 16\nuint16_t: 1600\nstring1: Hello\nstring2: there\nstring3: world!");
}

TEST(YamlSerialisationTests, CanEmittANodeAsAChildNode) {
    Node root{};

    Node parentNode{};
    parentNode["value1"] = 42;
    parentNode["value2"] = 100;

    root["ParentNode"] = parentNode;

    EXPECT_EQ(emittYaml(root), "type: yaml\nversion: 1\nParentNode:\n  value1: 42\n  value2: 100");

    root["OtherNode"] = 3;

    EXPECT_EQ(emittYaml(root), "type: yaml\nversion: 1\nParentNode:\n  value1: 42\n  value2: 100\nOtherNode: 3");
}

TEST(YamlSerialisationTests, CanEmittABasicSequenceNode) {
    Node root{};
    root["Sequence"].pushBack(1);
    root["Sequence"].pushBack(2);
    root["Sequence"].pushBack(3);

    EXPECT_EQ(emittYaml(root), "type: yaml\nversion: 1\nSequence:\n  - 1\n  - 2\n  - 3");
}

TEST(YamlSerialisationTests, CanEmittANestedSequenceNode) {
    Node node1{};
    node1["Val1"] = 1;
    Node node2{};
    node2["Val1"] = 1;
    node2["Val2"] = 2;

    Node root{};
    root["Sequence"].pushBack(node1);
    root["Sequence"].pushBack(node2);

    EXPECT_EQ(emittYaml(root), "type: yaml\nversion: 1\nSequence:\n  - Val1: 1\n  - Val1: 1\n    Val2: 2");

    root["Sequence"].pushBack(32);

    EXPECT_EQ(emittYaml(root), "type: yaml\nversion: 1\nSequence:\n  - Val1: 1\n  - Val1: 1\n    Val2: 2\n  - 32");
}

struct TestStruct {
    int32_t memberOne;
    int32_t memberTwo;
    float memberThree;
};

namespace clove {
    template<>
    Node serialise<TestStruct>(TestStruct const &object) {
        Node node{};
        node["memberOne"]   = object.memberOne;
        node["memberTwo"]   = object.memberTwo;
        node["memberThree"] = object.memberThree;
        return node;
    }
}

TEST(YamlSerialisationTests, CanEmittASerialisableStruct) {
    Node root{};

    TestStruct testStruct{
        .memberOne   = 1,
        .memberTwo   = 2,
        .memberThree = 3,
    };

    root["testStruct"] = testStruct;

    EXPECT_EQ(emittYaml(root), "type: yaml\nversion: 1\ntestStruct:\n  memberOne: 1\n  memberTwo: 2\n  memberThree: 3");
}

struct NestedTestStruct {
    float memberOne;
    TestStruct memberTwo;
};

namespace clove {
    template<>
    Node serialise<NestedTestStruct>(NestedTestStruct const &object) {
        Node node{};
        node["memberOne"] = object.memberOne;
        node["memberTwo"] = object.memberTwo;
        return node;
    }
}

TEST(YamlSerialisationTests, CanEmittANestedSerialisableStruct) {
    Node root{};

    NestedTestStruct testStruct{
        .memberOne = 1,
        .memberTwo = {
            .memberOne   = 1,
            .memberTwo   = 2,
            .memberThree = 3,
        },
    };

    root["testStruct"] = testStruct;

    EXPECT_EQ(emittYaml(root), "type: yaml\nversion: 1\ntestStruct:\n  memberOne: 1\n  memberTwo:\n    memberOne: 1\n    memberTwo: 2\n    memberThree: 3");
}

TEST(YamlSerialisationTests, CanEmittASerialisedSequenceNode) {
    TestStruct test1{
        .memberOne   = 1,
        .memberTwo   = 2,
        .memberThree = 32.5f,
    };

    TestStruct test2{
        .memberOne   = 4,
        .memberTwo   = 5,
        .memberThree = 64.5f,
    };

    Node root{};
    root["Sequence"].pushBack(test1);
    root["Sequence"].pushBack(test2);

    EXPECT_EQ(emittYaml(root), "type: yaml\nversion: 1\nSequence:\n  - memberOne: 1\n    memberTwo: 2\n    memberThree: 32.5\n  - memberOne: 4\n    memberTwo: 5\n    memberThree: 64.5");
}

TEST(YamlSerialisationTests, CanEmittAConvertedMapFromSequence) {
    Node root{};
    root["SeqToMap"].pushBack(1);
    root["SeqToMap"].pushBack(2);

    ASSERT_EQ(root["SeqToMap"].getType(), Node::Type::Sequence);
    EXPECT_EQ(emittYaml(root), "type: yaml\nversion: 1\nSeqToMap:\n  - 1\n  - 2");

    root["SeqToMap"]["Key"] = 100;

    EXPECT_EQ(root["SeqToMap"].getType(), Node::Type::Map);
    EXPECT_EQ(emittYaml(root), "type: yaml\nversion: 1\nSeqToMap:\n  0: 1\n  1: 2\n  Key: 100");
}

TEST(YamlSerialisationTests, CanEmittAMapThatIsPushedBack) {
    Node root{};
    root["Map"]["Val"] = 1;
    root["Map"]["Num"] = 2;

    ASSERT_EQ(root["Map"].getType(), Node::Type::Map);
    EXPECT_EQ(emittYaml(root), "type: yaml\nversion: 1\nMap:\n  Val: 1\n  Num: 2");

    root["Map"].pushBack(3);

    EXPECT_EQ(root["Map"].getType(), Node::Type::Map);
    EXPECT_EQ(emittYaml(root), "type: yaml\nversion: 1\nMap:\n  Val: 1\n  Num: 2\n  2: 3");
}