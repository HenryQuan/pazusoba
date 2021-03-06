#include <assert.h>
#include <fmt/core.h>
#include <pazusoba/core.h>

void testAll();
void testTimer();

int main() {
    using namespace std;
    testAll();
    fmt::print("All Good!\n");
}

void testAll() {
    testTimer();
}

void testTimer() {
    fmt::print("--- Test Timer ---\n");
    // timer crashes with O2
    pazusoba::Timer timer("=> Test Timer");
    int a = 0;
    int b = a + 99;
    int c = a + b + 1;
    for (int i = 0; i < 10; i++) {
        c++;
        fmt::print("c is now {}\n", c);
    }
}
