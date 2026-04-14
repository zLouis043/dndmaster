#pragma once
#include <string>
#include <vector>

// --- BUILDER PER 'CREATE TABLE' ---
class CreateTableQuery {
private:
    std::string tableName;
    std::vector<std::string> columns;
public:
    CreateTableQuery(const std::string& name) : tableName(name) {}
    CreateTableQuery& column(const std::string& name, const std::string& type) {
        columns.push_back(name + " " + type); return *this;
    }
    std::string build() const {
        std::string sql = "CREATE TABLE IF NOT EXISTS " + tableName + " (";
        for (size_t i = 0; i < columns.size(); ++i) {
            sql += columns[i]; if (i < columns.size() - 1) sql += ", ";
        }
        return sql + ");";
    }
};

// --- BUILDER PER 'INSERT' ---
class InsertQuery {
private:
    std::string tableName;
    std::vector<std::string> columns;
    bool ignore = false;
public:
    InsertQuery(const std::string& name) : tableName(name) {}
    InsertQuery& orIgnore() { ignore = true; return *this; }
    InsertQuery& column(const std::string& name) {
        columns.push_back(name); return *this;
    }
    std::string build() const {
        std::string sql = ignore ? "INSERT OR IGNORE INTO " : "INSERT INTO ";
        sql += tableName + " (";
        std::string placeholders = "VALUES (";
        for (size_t i = 0; i < columns.size(); ++i) {
            sql += columns[i]; placeholders += "?";
            if (i < columns.size() - 1) { sql += ", "; placeholders += ", "; }
        }
        return sql + ") " + placeholders + ")";
    }
};

// --- BUILDER PER 'SELECT' ---
class SelectQuery {
private:
    std::vector<std::string> columns;
    std::string table;
    std::vector<std::string> joins;
    std::string whereClause;
public:
    SelectQuery(const std::vector<std::string>& cols) : columns(cols) {}
    SelectQuery& from(const std::string& t) { table = t; return *this; }
    SelectQuery& join(const std::string& j) { joins.push_back(j); return *this; }
    SelectQuery& where(const std::string& w) { whereClause = w; return *this; }
    std::string build() const {
        std::string sql = "SELECT ";
        for (size_t i = 0; i < columns.size(); ++i) {
            sql += columns[i]; if (i < columns.size() - 1) sql += ", ";
        }
        sql += " FROM " + table;
        for (const auto& j : joins) sql += " JOIN " + j;
        if (!whereClause.empty()) sql += " WHERE " + whereClause;
        return sql;
    }
};

// --- BUILDER PER 'DELETE' ---
class DeleteQuery {
private:
    std::string table;
    std::string whereClause;
public:
    DeleteQuery(const std::string& t) : table(t) {}
    DeleteQuery& where(const std::string& w) { whereClause = w; return *this; }
    std::string build() const {
        std::string sql = "DELETE FROM " + table;
        if (!whereClause.empty()) sql += " WHERE " + whereClause;
        return sql;
    }
};

class UpdateQuery {
private:
    std::string tableName;
    std::vector<std::string> columns;
    std::string whereClause;
public:
    UpdateQuery(const std::string& name) : tableName(name) {}
    UpdateQuery& set(const std::string& col) { columns.push_back(col); return *this; }
    UpdateQuery& where(const std::string& w) { whereClause = w; return *this; }
    std::string build() const {
        std::string sql = "UPDATE " + tableName + " SET ";
        for (size_t i = 0; i < columns.size(); ++i) {
            sql += columns[i] + " = ?";
            if (i < columns.size() - 1) sql += ", ";
        }
        if (!whereClause.empty()) sql += " WHERE " + whereClause;
        return sql;
    }
};

// --- FACADE GLOBALE ---
class QueryBuilder {
public:
    static CreateTableQuery createTable(const std::string& tableName) { return CreateTableQuery(tableName); }
    static InsertQuery insertInto(const std::string& tableName) { return InsertQuery(tableName); }
    static SelectQuery select(const std::vector<std::string>& columns) { return SelectQuery(columns); }
    static DeleteQuery deleteFrom(const std::string& tableName) { return DeleteQuery(tableName); }
    static UpdateQuery update(const std::string& tableName) { return UpdateQuery(tableName); }
};