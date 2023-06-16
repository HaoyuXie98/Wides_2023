#ifndef PYTHON_API_H
#define PYTHON_API_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

std::map<std::string, std::string> python_api(std::string name, std::string args) 
{
    std::cout << "-------- Python application begin --------\n" << std::endl;

    // Prepare the base command
    std::ostringstream cmd;
    cmd << "python3 " + name + " " + args;

    std::cout << cmd.str().c_str();
    // Run the command
    int ret = system(cmd.str().c_str());

    std::cout << "\n-------- Python application end --------\n" << std::endl;
    std::cout << "Python script exited with status: " << (ret >> 8) << std::endl;

    std::ifstream file("output.txt");
    std::string line;
    std::map<std::string, std::string> data;
    while (std::getline(file, line)) {
        std::istringstream is_line(line);
        std::string key;
        if (std::getline(is_line, key, ',')) {
            std::string value;
            if (std::getline(is_line, value)) {
                data[key] = value;
            }
        }
    }

    // Print the data
    // std::cout << "return value:" << std::endl;
    // for (const auto &pair : data) {
    //     std::cout << pair.first << ": " << pair.second << std::endl;
    // }

    return data;
}

#endif