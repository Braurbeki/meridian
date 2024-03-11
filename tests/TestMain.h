#pragma once
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace mer::test {

struct Case {
    std::string           name;
    std::function<void()> body;
};

std::vector<Case>& registry();

struct Register {
    Register(std::string name, std::function<void()> body)
    {
        registry().push_back(Case{std::move(name), std::move(body)});
    }
};

void fail(const char* expression, const char* file, int line);

} // namespace mer::test

#define MER_CHECK(expr)                                                        \
    do {                                                                       \
        if (!(expr)) {                                                         \
            ::mer::test::fail(#expr, __FILE__, __LINE__);                      \
        }                                                                      \
    } while (0)

#define MER_CHECK_EQ(a, b)                                                     \
    do {                                                                       \
        const auto& lhs_ = (a);                                                \
        const auto& rhs_ = (b);                                                \
        if (!(lhs_ == rhs_)) {                                                 \
            std::cerr << "  got: " << lhs_ << "\n  want: " << rhs_ << "\n";    \
            ::mer::test::fail(#a " == " #b, __FILE__, __LINE__);               \
        }                                                                      \
    } while (0)

#define MER_TEST(name)                                                         \
    static void name();                                                        \
    static const ::mer::test::Register g_register_##name(#name, name);         \
    static void name()
