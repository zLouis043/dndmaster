#pragma once
#include "Inspection.h"
#include <vector>
#include <string>
#include <functional>

template <typename T>
class DyInspectable : public IInspectable {
public:
    using InspectLogic = std::function<void(IInspector&, T&)>;

private:
    InspectLogic m_logic;
    std::vector<T>* m_vectorData = nullptr;
    T* m_singleData = nullptr;

public:
    DyInspectable() = default;
    ~DyInspectable() override = default;

    std::string getType() const override { return "dynamic_inspectable"; }

    void define(InspectLogic logic) {
        m_logic = std::move(logic);
    }

    void bind(std::vector<T>& list) {
        m_vectorData = &list;
        m_singleData = nullptr;
    }

    void bind(T& item) {
        m_singleData = &item;
        m_vectorData = nullptr;
    }

    IInspectable* getInspectable() {
        return this;
    }

    void inspect(IInspector& inspector) override {
        if (!m_logic) return;

        struct ElementAdapter : public IInspectable {
            T& item;
            InspectLogic& logic;
            ElementAdapter(T& i, InspectLogic& l) : item(i), logic(l) {}
            std::string getType() const override { return "dynamic_element"; }
            void inspect(IInspector& ins) override {
                logic(ins, item);
            }
        };

        if (m_vectorData) {
            for (size_t i = 0; i < m_vectorData->size(); ++i) {
                ElementAdapter adapter((*m_vectorData)[i], m_logic);
                inspector.property("item_" + std::to_string(i), adapter)
                         .tag("ui-class", "dy-item");
            }
        } 
        else if (m_singleData) {
            m_logic(inspector, *m_singleData);
        }
    }
};