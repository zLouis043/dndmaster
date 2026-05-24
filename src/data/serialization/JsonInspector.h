#pragma once
#include "../inspector/Inspection.h"
#include <nlohmann/json.hpp>
#include <stack>

class JsonWriter : public IInspector {
private:
    std::stack<nlohmann::json*> m_stack;
    std::string m_pendingTypeToken = ""; 
    nlohmann::json m_defaultRoot;

public:
    JsonWriter() { 
        m_defaultRoot = nlohmann::json::object();
        m_stack.push(&m_defaultRoot); 
    }
    
    JsonWriter(nlohmann::json& dst) { 
        if (dst.is_null()) {
            dst = nlohmann::json::object();
        }
        m_stack.push(&dst); 
    }

    void name(const std::string&) override {}
    bool isReading() const override { return false; }

    void onPrimitive(const std::string& name, void* valuePtr, Type t, const PropertyTags& tags) override {
        nlohmann::json& curr = *m_stack.top();
        std::string key = tags.get<std::string>("json-name", name);

        auto assignVal = [&](auto& target) {
            switch (t) {
                case Type::INT:    target = *static_cast<int*>(valuePtr); break;
                case Type::FLOAT:  target = *static_cast<float*>(valuePtr); break;
                case Type::STRING: target = *static_cast<std::string*>(valuePtr); break;
                case Type::BOOL:   target = *static_cast<bool*>(valuePtr); break;
                case Type::UINT32: target = *static_cast<uint32_t*>(valuePtr); break;
            }
        };

        if (curr.is_array()) {
            nlohmann::json temp; assignVal(temp); curr.push_back(temp);
        } else {
            assignVal(curr[key]);
        }
    }

    void onInspectable(const std::string& name, IInspectable& value, const PropertyTags& tags) override {
        nlohmann::json& curr = *m_stack.top();
        std::string key = tags.get<std::string>("json-name", name);
        
        nlohmann::json subObj = nlohmann::json::object();
        
        if (!m_pendingTypeToken.empty()) {
            subObj["@class"] = m_pendingTypeToken; 
            m_pendingTypeToken.clear();
        }

        m_stack.push(&subObj);
        value.inspect(*this); 
        m_stack.pop();

        if (curr.is_array()) curr.push_back(subObj);
        else curr[key] = subObj;
    }

    void onBeginArray(const std::string& name, size_t&, const PropertyTags& tags) override {
        nlohmann::json& curr = *m_stack.top();
        std::string key = tags.get<std::string>("json-name", name);
        
        nlohmann::json newArray = nlohmann::json::array();
        if (curr.is_array()) {
            curr.push_back(newArray);
            m_stack.push(&curr.back());
        } else {
            curr[key] = newArray;
            m_stack.push(&curr[key]);
        }
    }
    
    void onArrayElement(size_t) override {} 
    void onEndArray() override { m_stack.pop(); }

    void onBeginPointer(const std::string&, std::string& typeToken, bool& isNull, const PropertyTags&) override {
        if (!isNull) m_pendingTypeToken = typeToken;
    }
    
    void onEndPointer() override { m_pendingTypeToken.clear(); }
};

class JsonReader : public IInspector {
private:
    std::stack<const nlohmann::json*> m_stack;
    size_t m_currentIndex = 0; 

public:
    JsonReader(const nlohmann::json& source) { m_stack.push(&source); }

    void name(const std::string&) override {}
    bool isReading() const override { return true; }

    const nlohmann::json* getTargetNode(const std::string& name, const PropertyTags& tags) {
        if (!m_stack.top()) return nullptr;
        const nlohmann::json& curr = *m_stack.top();
        
        if (curr.is_array()) {
            if (m_currentIndex < curr.size()) return &curr[m_currentIndex];
        } else {
            std::string key = tags.get<std::string>("json-name", name);
            if (curr.contains(key)) return &curr[key];
        }
        return nullptr;
    }

    void onPrimitive(const std::string& name, void* valuePtr, Type t, const PropertyTags& tags) override {
        const nlohmann::json* target = getTargetNode(name, tags);
        if (!target) return;

        try {
            switch (t) {
                case Type::INT:    *static_cast<int*>(valuePtr) = target->get<int>(); break;
                case Type::FLOAT:  *static_cast<float*>(valuePtr) = target->get<float>(); break;
                case Type::STRING: *static_cast<std::string*>(valuePtr) = target->get<std::string>(); break;
                case Type::BOOL:   *static_cast<bool*>(valuePtr) = target->get<bool>(); break;
                case Type::UINT32: *static_cast<uint32_t*>(valuePtr) = target->get<uint32_t>(); break;
            }
        } catch(...) {}
    }

    void onInspectable(const std::string& name, IInspectable& value, const PropertyTags& tags) override {
        const nlohmann::json* target = getTargetNode(name, tags);
        if (target && target->is_object()) {
            m_stack.push(target);
            value.inspect(*this);
            m_stack.pop();
        }
    }

    void onBeginArray(const std::string& name, size_t& size, const PropertyTags& tags) override {
        const nlohmann::json* target = getTargetNode(name, tags);
        if (target && target->is_array()) {
            size = target->size();
            m_stack.push(target);
        } else {
            size = 0;
            m_stack.push(nullptr); 
        }
    }

    void onArrayElement(size_t index) override { m_currentIndex = index; }
    void onEndArray() override { m_stack.pop(); }

    void onBeginPointer(const std::string& name, std::string& typeToken, bool& isNull, const PropertyTags& tags) override {
        const nlohmann::json* target = getTargetNode(name, tags);
        if (target && target->is_object() && target->contains("@class")) {
            typeToken = (*target)["@class"].get<std::string>();
            isNull = false;
        } else {
            isNull = true;
        }
    }

    void onEndPointer() override {}
};