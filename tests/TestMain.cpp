#include "TestMain.h"

#include <cstdlib>

namespace mer::test {

std::vector<Case>& registry()
{
    static std::vector<Case> cases;
    return cases;
}

void fail(const char* expression, const char* file, int line)
{
    std::cerr << "FAILED: " << expression << "\n  at " << file << ":" << line << "\n";
    std::exit(1);
}

} // namespace mer::test

int main()
{
    for (const auto& c : mer::test::registry()) {
        std::cout << "run " << c.name << "\n";
        c.body();
    }
    std::cout << mer::test::registry().size() << " test(s) passed\n";
    return 0;
}
