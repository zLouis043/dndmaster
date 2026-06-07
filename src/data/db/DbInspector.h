#pragma once
#include "../inspector/Inspection.h"
#include <SQLiteCpp/SQLiteCpp.h>
#include <vector>
#include <string>

class DbTableMapper : public IInspector {
public:
    struct Column { std::string name; std::string type; bool isPK = false; };
    std::string tableName;
    std::vector<Column> columns;
    int arrayIgnoreDepth = 0;

    void name(const std::string& n) override { tableName = n; }
    bool isReading() const override { return false; }

    void onPrimitive(const std::string& name, void* valuePtr, Type t, const PropertyTags& tags) override {
        if (arrayIgnoreDepth > 0 || tags.get<bool>("db-ignore", false)) return;
        
        std::string sqlT;
        switch (t) {
            case Type::INT: case Type::BOOL: case Type::UINT32: sqlT = "INTEGER"; break;
            case Type::FLOAT: sqlT = "REAL"; break;
            case Type::STRING: sqlT = "TEXT"; break;
        }
        columns.push_back({name, sqlT, tags.get<bool>("primary-key", false)});
    }

    void onInspectable(const std::string& name, IInspectable& value, const PropertyTags& tags) override {
        if (arrayIgnoreDepth > 0 || tags.get<bool>("db-ignore", false)) return;
        value.inspect(*this);
    }

    void onBeginArray(const std::string& name, size_t& size, const PropertyTags& tags) override { 
        #ifdef _DEBUG
        if (arrayIgnoreDepth == 0) {
            std::cerr << "[DB WARNING] Property '" << name 
                        << "' is an array and will be ignored by the Db Inspector. " << std::endl;
        }
        #endif
        arrayIgnoreDepth++;  
    }
    void onArrayElement(size_t) override {}
    void onEndArray() override { arrayIgnoreDepth--; }
    void onBeginPointer(const std::string& name, std::string& typeToken, bool& isNull, const PropertyTags& tags)  override { 
        #ifdef _DEBUG
            if (arrayIgnoreDepth == 0) {
                std::cerr << "[DB WARNING] Property '" << name 
                        << "' is a pointer and will be ignored by the Db Inspector. " << std::endl;
            }
        #endif
        arrayIgnoreDepth++; 
    }
    void onEndPointer() override { arrayIgnoreDepth--; }

    std::string createTableSQL() const {
        std::string sql = "CREATE TABLE IF NOT EXISTS " + tableName + " (";
        for (size_t i = 0; i < columns.size(); ++i) {
            sql += columns[i].name + " " + columns[i].type;
            if (columns[i].isPK) sql += " PRIMARY KEY AUTOINCREMENT";
            if (i < columns.size() - 1) sql += ", ";
        }
        sql += ");";
        return sql;
    }

    std::string insertSQL() const {
        std::string sql = "INSERT OR REPLACE INTO " + tableName + " (";
        std::string vals = "VALUES (";
        for (size_t i = 0; i < columns.size(); ++i) {
            sql += columns[i].name;
            vals += "?";
            if (i < columns.size() - 1) { sql += ", "; vals += ", "; }
        }
        return sql + ") " + vals + ");";
    }
};

class DbStatementBinder : public IInspector {
    SQLite::Statement& m_stmt;
    int m_index = 1;
    int arrayIgnoreDepth = 0;
public:
    DbStatementBinder(SQLite::Statement& stmt) : m_stmt(stmt) {}
    void name(const std::string&) override {}
    bool isReading() const override { return false; }

    void onPrimitive(const std::string& name, void* valuePtr, Type t, const PropertyTags& tags) override {
        if (arrayIgnoreDepth > 0 || tags.get<bool>("db-ignore", false)) return;

        if (t == Type::INT && tags.get<bool>("primary-key", false)) {
            int pkValue = *static_cast<int*>(valuePtr);
            if (pkValue <= 0) {
                m_stmt.bind(m_index++); 
                return;
            }
        }

        switch (t) {
            case Type::INT:    m_stmt.bind(m_index++, *static_cast<int*>(valuePtr)); break;
            case Type::FLOAT:  m_stmt.bind(m_index++, (double)*static_cast<float*>(valuePtr)); break;
            case Type::STRING: m_stmt.bind(m_index++, *static_cast<std::string*>(valuePtr)); break;
            case Type::BOOL:   m_stmt.bind(m_index++, *static_cast<bool*>(valuePtr) ? 1 : 0); break;
            case Type::UINT32: m_stmt.bind(m_index++, (long long)*static_cast<uint32_t*>(valuePtr)); break;
        }
    }
    void onInspectable(const std::string& name, IInspectable& value, const PropertyTags& tags) override {
        if (arrayIgnoreDepth > 0 || tags.get<bool>("db-ignore", false)) return;
        value.inspect(*this);
    }
    void onBeginArray(const std::string& name, size_t& size, const PropertyTags& tags) override { 
        #ifdef _DEBUG
        if (arrayIgnoreDepth == 0) {
            std::cerr << "[DB WARNING] Property '" << name 
                        << "' is an array and will be ignored by the Db Inspector. " << std::endl;
        }
        #endif
        arrayIgnoreDepth++;  
    }
    void onArrayElement(size_t) override {}
    void onEndArray() override { arrayIgnoreDepth--; }
    void onBeginPointer(const std::string& name, std::string& typeToken, bool& isNull, const PropertyTags& tags) override { 
        #ifdef _DEBUG
            if (arrayIgnoreDepth == 0) {
                std::cerr << "[DB WARNING] Property '" << name 
                        << "' is a pointer and will be ignored by the Db Inspector. " << std::endl;
            }
        #endif
        arrayIgnoreDepth++; 
    }
    void onEndPointer() override { arrayIgnoreDepth--; }
};

class DbRowExtractor : public IInspector {
    SQLite::Statement& m_stmt;
    int m_index = 0;
    int arrayIgnoreDepth = 0;
public:
    DbRowExtractor(SQLite::Statement& stmt) : m_stmt(stmt) {}
    void name(const std::string&) override {}
    bool isReading() const override { return true; }

    void onPrimitive(const std::string& name, void* valuePtr, Type t, const PropertyTags& tags) override {
        if (arrayIgnoreDepth > 0 || tags.get<bool>("db-ignore", false)) return;
        if (m_index >= m_stmt.getColumnCount()) return; 
        
        switch (t) {
            case Type::INT:    *static_cast<int*>(valuePtr) = m_stmt.getColumn(m_index++).getInt(); break;
            case Type::FLOAT:  *static_cast<float*>(valuePtr) = (float)m_stmt.getColumn(m_index++).getDouble(); break;
            case Type::STRING: *static_cast<std::string*>(valuePtr) = m_stmt.getColumn(m_index++).getText(); break;
            case Type::BOOL:   *static_cast<bool*>(valuePtr) = m_stmt.getColumn(m_index++).getInt() != 0; break;
            case Type::UINT32: *static_cast<uint32_t*>(valuePtr) = (uint32_t)m_stmt.getColumn(m_index++).getInt64(); break;
        }
    }
    void onInspectable(const std::string& name, IInspectable& value, const PropertyTags& tags) override {
        if (arrayIgnoreDepth > 0 || tags.get<bool>("db-ignore", false)) return;
        value.inspect(*this);
    }
    void onBeginArray(const std::string& name, size_t& size, const PropertyTags& tags) override { 
        #ifdef _DEBUG
        if (arrayIgnoreDepth == 0) {
            std::cerr << "[DB WARNING] Property '" << name 
                        << "' is an array and will be ignored by the Db Inspector. " << std::endl;
        }
        #endif
        arrayIgnoreDepth++;  
    }
    void onArrayElement(size_t) override {}
    void onEndArray() override { arrayIgnoreDepth--; }
    void onBeginPointer(const std::string& name, std::string& typeToken, bool& isNull, const PropertyTags& tags) override { 
        #ifdef _DEBUG
            if (arrayIgnoreDepth == 0) {
                std::cerr << "[DB WARNING] Property '" << name 
                        << "' is a pointer and will be ignored by the Db Inspector. " << std::endl;
            }
        #endif
        arrayIgnoreDepth++; 
    }
    void onEndPointer() override { arrayIgnoreDepth--; }
};