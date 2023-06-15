#include <iostream>
#include "python_api_test.h"

int main(int argc, char *argv[]) {
    std::string func = "test_1", args = "1 10 0";
    python_api(func, args);
    return 0;
}