/**
 * @file policy_chain_test.cpp
 * @brief Comprehensive unit tests for cf::PolicyChain using GoogleTest
 *
 * Test Coverage:
 * 1. Basic Construction and Operations
 * 2. Fallback Mechanism
 * 3. Multiple Arguments
 * 4. Factory Functions
 * 5. Builder API
 * 6. Edge Cases
 * 7. Empty Chain Behavior
 */

#include "base/policy_chain/policy_chain.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

// =============================================================================
// Test Suite 1: Basic Construction and Operations
// =============================================================================

TEST(PolicyChainTest, DefaultConstructor) {
    cf::PolicyChain<int, int> chain;

    EXPECT_TRUE(chain.empty());
    EXPECT_EQ(chain.size(), 0);
    EXPECT_FALSE(chain.execute(42).has_value());
}

TEST(PolicyChainTest, AddFront) {
    cf::PolicyChain<int, int> chain;

    chain.add_front([](int x) -> std::optional<int> { return x * 2; });

    EXPECT_FALSE(chain.empty());
    EXPECT_EQ(chain.size(), 1);
    EXPECT_EQ(chain.execute(5), 10);
}

TEST(PolicyChainTest, AddBack) {
    cf::PolicyChain<int, int> chain;

    chain.add_back([](int x) -> std::optional<int> { return x * 2; });

    EXPECT_FALSE(chain.empty());
    EXPECT_EQ(chain.size(), 1);
    EXPECT_EQ(chain.execute(5), 10);
}

TEST(PolicyChainTest, Clear) {
    cf::PolicyChain<int, int> chain;

    chain.add_back([](int) -> std::optional<int> { return 1; });
    chain.add_back([](int) -> std::optional<int> { return 2; });

    EXPECT_EQ(chain.size(), 2);

    chain.clear();

    EXPECT_TRUE(chain.empty());
    EXPECT_EQ(chain.size(), 0);
    EXPECT_FALSE(chain.execute(0).has_value());
}

TEST(PolicyChainTest, FunctionCallOperator) {
    cf::PolicyChain<int, int> chain;

    chain.add_back([](int x) -> std::optional<int> { return x + 10; });

    // Test operator() overload
    auto result = chain(5);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(*result, 15);
}

// =============================================================================
// Test Suite 2: Fallback Mechanism
// =============================================================================

TEST(PolicyChainTest, Fallback_SecondPolicy) {
    cf::PolicyChain<int, int> chain;

    // First policy fails for negative input
    chain.add_front([](int x) -> std::optional<int> {
        if (x > 0)
            return x * 2;
        return std::nullopt;
    });

    // Second policy handles negative input
    chain.add_back([](int x) -> std::optional<int> {
        if (x < 0)
            return -x;
        return std::nullopt;
    });

    EXPECT_EQ(chain.execute(5), 10); // First policy
    EXPECT_EQ(chain.execute(-3), 3); // Fallback to second policy
}

TEST(PolicyChainTest, Fallback_MultiplePolicies) {
    cf::PolicyChain<int, int> chain;

    chain.add_front([](int x) -> std::optional<int> {
        if (x > 100)
            return x;
        return std::nullopt;
    });

    chain.add_back([](int x) -> std::optional<int> {
        if (x > 50)
            return x * 2;
        return std::nullopt;
    });

    chain.add_back([](int x) -> std::optional<int> {
        if (x > 0)
            return x + 10;
        return std::nullopt;
    });

    chain.add_back([](int) -> std::optional<int> {
        return 0; // Default fallback
    });

    EXPECT_EQ(chain.execute(150), 150); // First policy
    EXPECT_EQ(chain.execute(75), 150);  // Second policy: 75 * 2
    EXPECT_EQ(chain.execute(25), 35);   // Third policy: 25 + 10
    EXPECT_EQ(chain.execute(-5), 0);    // Fourth policy (default)
}

TEST(PolicyChainTest, Fallback_AllFail) {
    cf::PolicyChain<int, int> chain;

    chain.add_front([](int) -> std::optional<int> { return std::nullopt; });

    chain.add_back([](int) -> std::optional<int> { return std::nullopt; });

    auto result = chain.execute(42);
    EXPECT_FALSE(result.has_value());
}

TEST(PolicyChainTest, Fallback_Order_AddFront) {
    cf::PolicyChain<int, int> chain;

    chain.add_front([](int) -> std::optional<int> { return 1; });
    chain.add_front([](int) -> std::optional<int> { return 2; });
    chain.add_front([](int) -> std::optional<int> { return 3; });

    // Last add_front is at the head (highest priority)
    EXPECT_EQ(chain.execute(0), 3);
}

TEST(PolicyChainTest, Fallback_Order_AddBack) {
    cf::PolicyChain<int, int> chain;

    chain.add_back([](int) -> std::optional<int> { return 1; });
    chain.add_back([](int) -> std::optional<int> { return 2; });
    chain.add_back([](int) -> std::optional<int> { return 3; });

    // First add_back is at the head (highest priority)
    EXPECT_EQ(chain.execute(0), 1);
}

// =============================================================================
// Test Suite 3: Multiple Arguments
// =============================================================================

TEST(PolicyChainTest, MultipleArguments_TwoArgs) {
    cf::PolicyChain<int, int, int> chain;

    chain.add_back([](int a, int b) -> std::optional<int> {
        if (b != 0)
            return a / b;
        return std::nullopt;
    });

    chain.add_back([](int a, int) -> std::optional<int> { return a * 2; });

    EXPECT_EQ(chain.execute(10, 2), 5);
    EXPECT_EQ(chain.execute(10, 0), 20);
}

TEST(PolicyChainTest, MultipleArguments_ThreeArgs) {
    cf::PolicyChain<int, int, int, int> chain;

    chain.add_back([](int a, int b, int c) -> std::optional<int> { return a + b + c; });

    EXPECT_EQ(chain.execute(1, 2, 3), 6);
}

TEST(PolicyChainTest, MultipleArguments_StringRef) {
    cf::PolicyChain<int, const std::string&> chain;

    chain.add_back([](const std::string& s) -> std::optional<int> {
        if (!s.empty())
            return static_cast<int>(s.length());
        return std::nullopt;
    });

    chain.add_back([](const std::string&) -> std::optional<int> { return -1; });

    EXPECT_EQ(chain.execute("hello"), 5);
    EXPECT_EQ(chain.execute(""), -1);
}

// =============================================================================
// Test Suite 4: Factory Functions
// =============================================================================

TEST(PolicyChainTest, MakePolicyChain_Basic) {
    auto chain = cf::make_policy_chain<int, int>(
        [](int x) -> std::optional<int> {
            if (x > 0)
                return x * 2;
            return std::nullopt;
        },
        [](int x) -> std::optional<int> { return -x; });

    EXPECT_EQ(chain.execute(5), 10);
    EXPECT_EQ(chain.execute(-3), 3);
}

TEST(PolicyChainTest, MakePolicyChain_MultiplePolicies) {
    auto chain = cf::make_policy_chain<int, const std::string&>(
        [](const std::string& s) -> std::optional<int> {
            try {
                return std::stoi(s);
            } catch (...) {
                return std::nullopt;
            }
        },
        [](const std::string& s) -> std::optional<int> {
            if (!s.empty())
                return static_cast<int>(s.length());
            return std::nullopt;
        },
        [](const std::string&) -> std::optional<int> { return -1; });

    EXPECT_EQ(chain.execute("123"), 123);
    EXPECT_EQ(chain.execute("hello"), 5);
    EXPECT_EQ(chain.execute(""), -1);
}

// =============================================================================
// Test Suite 5: Builder API
// =============================================================================

TEST(PolicyChainTest, Builder_Basic) {
    auto chain = cf::policy_chain_builder<int, int>()
                     .then([](int x) -> std::optional<int> {
                         if (x > 0)
                             return x * 2;
                         return std::nullopt;
                     })
                     .then([](int x) -> std::optional<int> { return -x; })
                     .build();

    EXPECT_EQ(chain.execute(5), 10);
    EXPECT_EQ(chain.execute(-3), 3);
}

TEST(PolicyChainTest, Builder_ChainedThen) {
    auto chain = cf::policy_chain_builder<int, double>()
                     .then([](double x) -> std::optional<int> {
                         if (x >= 0.0)
                             return static_cast<int>(x + 0.5);
                         return std::nullopt;
                     })
                     .then([](double x) -> std::optional<int> { return static_cast<int>(x); })
                     .then([](double) -> std::optional<int> { return 0; })
                     .build();

    EXPECT_EQ(chain.execute(3.7), 4);
    EXPECT_EQ(chain.execute(-2.3), -2);
}

TEST(PolicyChainTest, Builder_MoveBuild) {
    auto builder =
        cf::policy_chain_builder<int, int>().then([](int x) -> std::optional<int> { return x; });

    auto chain = std::move(builder).build();
    EXPECT_EQ(chain.execute(42), 42);
}

TEST(PolicyChainTest, Builder_CopyBuild) {
    auto builder =
        cf::policy_chain_builder<int, int>().then([](int x) -> std::optional<int> { return x; });

    auto chain = builder.build();
    EXPECT_EQ(chain.execute(42), 42);
}

// =============================================================================
// Test Suite 6: Edge Cases
// =============================================================================

TEST(PolicyChainEdgeCases, EmptyChain) {
    cf::PolicyChain<int, int> chain;

    auto result = chain.execute(42);
    EXPECT_FALSE(result.has_value());
}

TEST(PolicyChainEdgeCases, SinglePolicy) {
    cf::PolicyChain<int, int> chain;

    chain.add_back([](int x) -> std::optional<int> { return x + 1; });

    EXPECT_EQ(chain.execute(5), 6);
}

TEST(PolicyChainEdgeCases, OptionalBool) {
    cf::PolicyChain<bool, int> chain;

    chain.add_back([](int x) -> std::optional<bool> {
        if (x > 100)
            return true;
        return std::nullopt;
    });

    chain.add_back([](int x) -> std::optional<bool> {
        if (x > 50)
            return false;
        return std::nullopt;
    });

    chain.add_back([](int) -> std::optional<bool> { return false; });

    EXPECT_EQ(chain.execute(150), true);
    EXPECT_EQ(chain.execute(75), false);
    EXPECT_EQ(chain.execute(10), false);
}

TEST(PolicyChainEdgeCases, MoveOnlyReturnType) {
    cf::PolicyChain<std::unique_ptr<int>, int> chain;

    chain.add_back(
        [](int x) -> std::optional<std::unique_ptr<int>> { return std::make_unique<int>(x * 2); });

    auto result = chain.execute(5);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(**result, 10);
}

TEST(PolicyChainEdgeCases, StringReturnType) {
    cf::PolicyChain<std::string, int> chain;

    chain.add_back([](int x) -> std::optional<std::string> {
        if (x > 0)
            return "positive";
        return std::nullopt;
    });

    chain.add_back([](int x) -> std::optional<std::string> {
        if (x < 0)
            return "negative";
        return std::nullopt;
    });

    chain.add_back([](int) -> std::optional<std::string> { return "zero"; });

    EXPECT_EQ(chain.execute(5), "positive");
    EXPECT_EQ(chain.execute(-5), "negative");
    EXPECT_EQ(chain.execute(0), "zero");
}

TEST(PolicyChainEdgeCases, ConstPolicyChain) {
    cf::PolicyChain<int, int> chain;

    chain.add_back([](int x) -> std::optional<int> { return x * 2; });

    const auto& const_chain = chain;
    auto result = const_chain.execute(5);
    EXPECT_EQ(result, 10);
}

TEST(PolicyChainEdgeCases, PolicyWithSideEffects) {
    int call_count = 0;

    cf::PolicyChain<int, int> chain;

    chain.add_back([&call_count](int x) -> std::optional<int> {
        ++call_count;
        if (x > 0)
            return x;
        return std::nullopt;
    });

    chain.add_back([&call_count](int x) -> std::optional<int> {
        ++call_count;
        return -x;
    });

    call_count = 0;
    EXPECT_EQ(chain.execute(5), 5);
    EXPECT_EQ(call_count, 1); // Only first policy called

    call_count = 0;
    EXPECT_EQ(chain.execute(-5), 5);
    EXPECT_EQ(call_count, 2); // Both policies called
}

// =============================================================================
// Test Suite 7: Range-based Iteration
// =============================================================================

TEST(PolicyChainTest, RangeBasedFor) {
    cf::PolicyChain<int, int> chain;

    chain.add_back([](int) -> std::optional<int> { return 1; });
    chain.add_back([](int) -> std::optional<int> { return 2; });
    chain.add_back([](int) -> std::optional<int> { return 3; });

    int count = 0;
    for (const auto& policy : chain) {
        (void)policy; // Suppress unused warning
        ++count;
    }

    EXPECT_EQ(count, 3);
}

TEST(PolicyChainTest, BeginEnd) {
    cf::PolicyChain<int, int> chain;

    chain.add_back([](int) -> std::optional<int> { return 1; });
    chain.add_back([](int) -> std::optional<int> { return 2; });

    auto begin = chain.begin();
    auto end = chain.end();

    EXPECT_NE(begin, end);
    EXPECT_EQ(chain.size(), std::distance(begin, end));
}

// =============================================================================
// Test Suite 8: Type Traits
// =============================================================================

TEST(PolicyChainTest, TypeTraits) {
    using Chain = cf::PolicyChain<int, const std::string&>;

    EXPECT_TRUE((std::is_copy_constructible_v<Chain>));
    EXPECT_TRUE((std::is_move_constructible_v<Chain>));
    EXPECT_TRUE((std::is_copy_assignable_v<Chain>));
    EXPECT_TRUE((std::is_move_assignable_v<Chain>));

    EXPECT_TRUE((std::is_same_v<typename Chain::ValueType, int>));
    EXPECT_TRUE((std::is_same_v<typename Chain::PolicyType,
                                std::function<std::optional<int>(const std::string&)>>));
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
