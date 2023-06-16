#include <iostream>
#include "python_api_test.h"
#include <map>
#include <string>


int main(int argc, char *argv[]) {
    std::string name = "../system_method/test_1", args = "30 10 25";
    std::map<std::string, std::string> ret = python_api(name, args);
    // Output the key-value pairs of the ret to make sure 
    for (const auto& pair : ret) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
    return 0;
}