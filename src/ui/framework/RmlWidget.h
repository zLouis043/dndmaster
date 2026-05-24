#pragma once
#include <RmlUi/Core/Element.h>
#include <string>
#include <fstream>
#include <sstream>

class RmlWidget {
public:
    static std::string LoadRmlFromFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) return "<div>Error: RML not found</div>";
        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }
};