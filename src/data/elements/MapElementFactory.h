#pragma once
#include "IMapElement.h"
#include "../inspector/Inspection.h"
#include <map>
#include <string>
#include <functional>
#include <memory>
#include <iostream>

class MapElementFactory {
public:
    using CreatorFunc = std::function<std::shared_ptr<IMapElement>()>;

    template<typename T>
    static void registerType(const std::string& typeName) {
        getRegistry()[typeName] = []() -> std::shared_ptr<IMapElement> {
            return std::make_shared<T>();
        };
    }

    static std::shared_ptr<IMapElement> create(const std::string& typeName) {
        auto& reg = getRegistry();
        if (reg.find(typeName) != reg.end()) return reg[typeName]();
        std::cerr << "[FACTORY] Errore: Tipo '" << typeName << "' non registrato!" << std::endl;
        return nullptr;
    }

private:
    static std::map<std::string, CreatorFunc>& getRegistry() {
        static std::map<std::string, CreatorFunc> registry;
        return registry;
    };
};

template <>
struct ObjectFactory<IMapElement> {
    static std::shared_ptr<IMapElement> create(const std::string& token) {
        return MapElementFactory::create(token);
    }
};

template<typename T>
struct AutoRegisterMapElement {
    AutoRegisterMapElement(const std::string& typeName) { MapElementFactory::registerType<T>(typeName); }
};

#define REGISTER_MAP_ELEMENT(TYPE_CLASS, TYPE_STRING) \
    namespace { static AutoRegisterMapElement<TYPE_CLASS> reg_##TYPE_CLASS(TYPE_STRING); };