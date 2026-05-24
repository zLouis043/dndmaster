#pragma once
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <memory>
#include <type_traits>
#include <functional>

template<typename T> struct is_vector : std::false_type {};
template<typename T, typename A> struct is_vector<std::vector<T, A>> : std::true_type {};
template<typename T> inline constexpr bool is_vector_v = is_vector<T>::value;

template<typename T> struct is_shared_ptr : std::false_type {};
template<typename T> struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};
template<typename T> inline constexpr bool is_shared_ptr_v = is_shared_ptr<T>::value;

template <typename Base>
struct ObjectFactory {
    static std::shared_ptr<Base> create(const std::string& token) { return nullptr; }
};

class PropertyTags {
    std::unordered_map<std::string, std::variant<bool, int, float, std::string>> m_tags;
public:
    template <typename V> void set(const std::string& key, V&& value) { 
        using DecayedV = std::decay_t<V>;
        if constexpr (std::is_same_v<DecayedV, const char*> || std::is_same_v<DecayedV, char*>) {
            m_tags[key] = std::string(value);
        } else {
            m_tags[key] = std::forward<V>(value); 
        }
    }
    
    template <typename T> T get(const std::string& key, T fallback) const {
        auto it = m_tags.find(key);
        if (it != m_tags.end() && std::holds_alternative<T>(it->second)) return std::get<T>(it->second);
        return fallback;
    }
    
    void append(const PropertyTags& other) {
        for (auto& pair : other.m_tags) m_tags[pair.first] = pair.second;
    }
};

enum class Type { INT, FLOAT, STRING, BOOL, UINT32 };

class IInspectable;
template <typename T> struct TypeCodec;
template <typename T, typename AsType> struct TypeConverter;

class IInspector {
public:
    virtual ~IInspector() = default;
    virtual void name(const std::string& n) = 0;
    virtual bool isReading() const = 0;
    
    virtual void onPrimitive(const std::string& name, void* valuePtr, Type t, const PropertyTags& tags) = 0;
    virtual void onInspectable(const std::string& name, class IInspectable& value, const PropertyTags& tags) = 0;
    virtual void onBeginArray(const std::string& name, size_t& size, const PropertyTags& tags) = 0;
    virtual void onArrayElement(size_t index) = 0;
    virtual void onEndArray() = 0;
    virtual void onBeginPointer(const std::string& name, std::string& typeToken, bool& isNull, const PropertyTags& tags) = 0;
    virtual void onEndPointer() = 0;
    virtual void onAction(const std::string& name, std::function<void()>& action, const PropertyTags& tags) {}

    template <typename T>
    void resolveProperty(const std::string& name, T& value, const PropertyTags& tags) {
        if constexpr (is_vector_v<T>) {
            size_t size = value.size();
            onBeginArray(name, size, tags);
            if (isReading()) value.resize(size);
            for (size_t i = 0; i < size; ++i) {
                onArrayElement(i);
                resolveProperty(std::to_string(i), value[i], tags);
            }
            onEndArray();
        }
        else if constexpr (is_shared_ptr_v<T>) {
            using ElementType = typename T::element_type;
            std::string token = "";
            bool isNull = (value == nullptr);
            if (!isNull) token = value->getType();

            onBeginPointer(name, token, isNull, tags);
            if (isReading()) {
                if (!isNull) value = std::dynamic_pointer_cast<ElementType>(ObjectFactory<ElementType>::create(token));
                else value.reset();
            }
            if (value) onInspectable(name, *value, tags);
            onEndPointer();
        }
        else if constexpr (std::is_same_v<T, int>) onPrimitive(name, &value, Type::INT, tags);
        else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) onPrimitive(name, &value, Type::FLOAT, tags);
        else if constexpr (std::is_same_v<T, std::string>) onPrimitive(name, &value, Type::STRING, tags);
        else if constexpr (std::is_same_v<T, bool>) onPrimitive(name, &value, Type::BOOL, tags);
        else if constexpr (std::is_same_v<T, uint32_t>) onPrimitive(name, &value, Type::UINT32, tags);
        else if constexpr (std::is_same_v<T, std::function<void()>>) onAction(name, value, tags);
        else if constexpr (std::is_base_of_v<IInspectable, T>) onInspectable(name, value, tags);
        else {
            if (isReading()) TypeCodec<T>::read(*this, name, value, tags);
            else TypeCodec<T>::write(*this, name, value, tags);
        }
    }
    
    template <typename T, typename AsType = T>
    struct Proxy {
        IInspector* ins; std::string n; T* v; PropertyTags t;
        Proxy(IInspector* i, std::string name, T* val) : ins(i), n(std::move(name)), v(val) {}
        template <typename V> Proxy& tag(const std::string& k, V&& val) { t.set(k, std::forward<V>(val)); return *this; }
        Proxy& tags(const PropertyTags& o) { t.append(o); return *this; }
        
        template <typename NewAsType>
        Proxy<T, NewAsType> as() {
            Proxy<T, NewAsType> p(ins, n, v); p.t = std::move(this->t);
            this->ins = nullptr; 
            return p;
        }

        ~Proxy() { 
            if (!ins) return;
            if constexpr (std::is_same_v<T, AsType>) {
                ins->resolveProperty(n, *v, t);
            } else {
                if (ins->isReading()) {
                    AsType temp;
                    TypeCodec<AsType>::read(*ins, n, temp, t);
                    TypeConverter<T, AsType>::convert(temp, *v);
                } else {
                    AsType temp;
                    TypeConverter<T, AsType>::convert(*v, temp);
                    TypeCodec<AsType>::write(*ins, n, temp, t);
                }
            }
        }
    };

    template <typename T>
    Proxy<T, T> property(const std::string& name, T& value) {
        return Proxy<T, T>(this, name, &value);
    }
};

class IInspectable {
public:
    virtual ~IInspectable() = default;
    virtual std::string getType() const { return "inspectable"; }
    virtual void inspect(IInspector& inspector) = 0;
};

template <typename Original, typename Serialized, typename WriterType, typename ReaderType>
struct AutoConverter {
    static void convert(const Original& in, Serialized& out) {
        WriterType writer(out);
        if constexpr (std::is_base_of_v<IInspectable, Original>) {
            const_cast<Original&>(in).inspect(writer);
        } else {
            writer.resolveProperty("payload", const_cast<Original&>(in), PropertyTags{});
        }
    }
    static void convert(const Serialized& in, Original& out) {
        ReaderType reader(const_cast<Serialized&>(in));
        if constexpr (std::is_base_of_v<IInspectable, Original>) {
            out.inspect(reader);
        } else {
            reader.resolveProperty("payload", out, PropertyTags{});
        }
    }
};