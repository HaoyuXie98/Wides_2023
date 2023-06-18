#include <iostream>
#include "python_api_test.h"
#include <map>
#include <string>


int main(int argc, char *argv[]) {
    std::string name = "status.py", args = "";
    std::map<std::string, std::string> ret1 = python_api(name, args);
    // Output the key-value pairs of the ret to make sure 
    for (const auto& pair : ret1) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }

    name = "move.py";
    args = "--x 550 --y 70";
    std::map<std::string, std::string> ret2 = python_api(name, args);
    // Output the key-value pairs of the ret to make sure 
    for (const auto& pair : ret2) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }

    name = "status.py";
    args = "";
    std::map<std::string, std::string> ret3 = python_api(name, args);
    // Output the key-value pairs of the ret to make sure 
    for (const auto& pair : ret3) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
    return 0;
}
