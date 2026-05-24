#pragma once
#include "../inspector/Inspection.h"
#include "../serialization/JsonInspector.h"
#include <nlohmann/json.hpp>

template <typename T>
struct TypeConverter<T, nlohmann::json> : AutoConverter<T, nlohmann::json, JsonWriter, JsonReader> {};

template <>
struct TypeCodec<nlohmann::json> {
    static void write(IInspector& ins, const std::string& name, nlohmann::json& value, const PropertyTags& tags) {
        std::string s = value.dump();
        ins.property(name, s).tags(tags); 
    }
    static void read(IInspector& ins, const std::string& name, nlohmann::json& value, const PropertyTags& tags) {
        std::string s;
        ins.property(name, s).tags(tags);
        if (!s.empty() && s != "{}") {
            try { value = nlohmann::json::parse(s); } catch (...) {}
        }
    }
};