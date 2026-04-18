
#ifndef PROJECT_ROOT
#define PROJECT_ROOT "."
#endif

#ifndef MY_HEADER_H   
#define MY_HEADER_H
#include <curl/curl.h>
#include <bits/stdc++.h>
using namespace std;
#include "../../json.hpp"
using json = nlohmann::json;


#include <iostream>
#include <filesystem>
#include <string>
using namespace std;

namespace fs = std::filesystem;

void setFsPointer(string path,bool print=true) {
try {
        string final_path = PROJECT_ROOT+path;
        fs::path targetPath(final_path);

        if (fs::exists(targetPath)) {
            fs::current_path(targetPath);
            if(print) std::cout << "Working directory set to: " << fs::current_path() << std::endl;
        } else {
            std::cerr << "Path does not exist: " << targetPath << std::endl;
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error changing directory: " << e.what() << std::endl;
    }
}





#endif

