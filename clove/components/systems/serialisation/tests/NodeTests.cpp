#include <Clove/Serialisation/Node.hpp>
#include <gtest/gtest.h>

using namespace clove;
using namespace clove::serialiser;

TEST(NodeTests, CanDefaultInitialiseNode) {
    Node node{};

    EXPECT_EQ(node.getType(), Node::Type::None);
    EXPECT_EQ(node.numChildren(), 0);
}

TEST(NodeTests, CanSetAndGetAValueOnANode) {
    Node node{};

    node = 5;
    EXPECT_EQ(node.getType(), Node::Type::Scalar);
    EXPECT_EQ(node.numChildren(), 1);
    EXPECT_EQ(node.as<uint32_t>(), 5);

    uint64_t max64{ ~0u };
    node = max64;
    EXPECT_EQ(node.as<uint64_t>(), max64);

    node = 3.5555f;
    EXPECT_FLOAT_EQ(node.as<float>(), 3.5555f);

    node = "test";
    EXPECT_EQ(node.as<std::string>(), "test");
}

TEST(NodeTests, CanAddNamedChildOntoNode) {
    Node node{};

    node["child"] = 3;

    EXPECT_EQ(node.getType(), Node::Type::Map);
    EXPECT_EQ(node.numChildren(), 1);
}

TEST(NodeTests, CanAddPushBackChildOntoNode) {
    Node node{};

    node.pushBack(3);

    EXPECT_EQ(node.getType(), Node::Type::Sequence);
    EXPECT_EQ(node.numChildren(), 1);
}

TEST(NodeTests, CanRetrieveANamedValueFromANode) {
    Node node{};

    node["child"] = 3;

    EXPECT_EQ(node.getType(), Node::Type::Map);
    EXPECT_EQ(node.numChildren(), 1);
    EXPECT_EQ(node["child"].as<int32_t>(), 3);
}

TEST(NodeTests, CanRetrieveAnIndexedValueFromANode) {
    Node node{};

    node.pushBack(100);

    EXPECT_EQ(node.getType(), Node::Type::Sequence);
    EXPECT_EQ(node.numChildren(), 1);
    EXPECT_EQ(node[0].as<int32_t>(), 100);
}

TEST(NodeTests, CanIterateThroughANodeUsingIterators) {
    Node map{};
    map["val1"] = 0;
    map["val2"] = "test";
    map["val3"] = 35.5f;

    for(size_t i{ 0 }; auto const &node : map) {
        switch(i) {
            case 0:
                EXPECT_EQ(node.as<int32_t>(), 0);
                break;
            case 1:
                EXPECT_EQ(node.as<std::string>(), "test");
                break;
            case 2:
                EXPECT_EQ(node.as<float>(), 35.5f);
                break;
            default:
                FAIL();
                break;
        }

        ++i;
    }
}

TEST(NodeTests, CanIterateThroughANodeUsingIndices) {
    Node seq{};
    seq.pushBack(0);
    seq.pushBack("test");
    seq.pushBack(35.5f);

    for(size_t i{ 0 }; i < seq.numChildren(); ++i) {
        switch(i) {
            case 0:
                EXPECT_EQ(seq[i].as<int32_t>(), 0);
                break;
            case 1:
                EXPECT_EQ(seq[i].as<std::string>(), "test");
                break;
            case 2:
                EXPECT_EQ(seq[i].as<float>(), 35.5f);
                break;
            default:
                FAIL();
                break;
        }
    }
}

TEST(NodeTests, CannotConvertAScalarIntoASequence) {
    Node root{};
    root["ScalarToSequence"] = 1;

    ASSERT_EQ(root["ScalarToSequence"].getType(), Node::Type::Scalar);

    EXPECT_ANY_THROW(root["ScalarToSequence"].pushBack(2));
}

TEST(NodeTests, CanConvertASequenceIntoAMap) {
    Node root{};
    root["SeqToMap"].pushBack(1);
    root["SeqToMap"].pushBack(2);

    ASSERT_EQ(root["SeqToMap"].getType(), Node::Type::Sequence);

    root["SeqToMap"]["Key"] = 100;

    EXPECT_EQ(root["SeqToMap"].getType(), Node::Type::Map);
}

TEST(NodeTests, CanPushBackOntoAMapNode) {
    Node root{};
    root["Map"]["Val"] = 1;
    root["Map"]["Num"] = 2;

    ASSERT_EQ(root["Map"].getType(), Node::Type::Map);

    root["Map"].pushBack(3);

    EXPECT_EQ(root["Map"].getType(), Node::Type::Map);
}

TEST(NodeTests, WontIterateEmptyNode) {
    bool loopedEmpty{ false };

    Node empty{};
    for(auto &node : empty){
        loopedEmpty = true;
        break;
    }

    EXPECT_FALSE(loopedEmpty);

    bool loopedEmptyChild{ false };

    for(auto &node : empty["child"]) {
        loopedEmptyChild = true;
        break;
    }

    EXPECT_FALSE(loopedEmptyChild);
}