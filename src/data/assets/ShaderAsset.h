#pragma once
#include "../../core/assets/AssetManager.h"
#include <include/effects/SkRuntimeEffect.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

template <>
struct AssetTraits<sk_sp<SkRuntimeEffect>> {
    
    static sk_sp<SkRuntimeEffect> load(const std::string& path) {
        std::ifstream file(path);
        if (!file) {
            std::cerr << "[PLUG SHADER] Errore: Impossibile trovare o aprire il file shader: " << path << std::endl;
            return nullptr;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return compile(buffer.str());
    }

    static sk_sp<SkRuntimeEffect> loadFromMemory(const std::vector<uint8_t>& data) {
        std::string code(data.begin(), data.end());
        return compile(code);
    }

private:
    static sk_sp<SkRuntimeEffect> compile(const std::string& code) {
        auto result = SkRuntimeEffect::MakeForShader(SkString(code.c_str()));
        if (!result.errorText.isEmpty()) {
            std::cerr << "[PLUG SHADER] Errore Critico di Compilazione SkSL:\n" << result.errorText.c_str() << std::endl;
            return nullptr;
        }
        return result.effect;
    }
};