#pragma once
#include <SQLiteCpp/SQLiteCpp.h>
#include <memory>
#include <iostream>
#include <vector>
#include "DbInspector.h"
#include "DbRegistry.h"

class DatabaseEngine {
private:
    std::unique_ptr<SQLite::Database> db;

public:
    bool open(const std::string& path) {
        try {
            db = std::make_unique<SQLite::Database>(path, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
            for (const auto& registerFunc : DbSchemaRegistry::getSchemas()) registerFunc(*this);
            return true;
        } catch (std::exception& e) { return false; }
    }

    template <typename T>
    bool registerSchema() {
        T entity; DbTableMapper mapper; entity.inspect(mapper);
        try { db->exec(mapper.createTableSQL()); return true; } 
        catch (std::exception& e) { return false; }
    }

    bool createJunctionTable(const std::string& tableName, const std::string& fk1, const std::string& fk2) {
        std::string sql = "CREATE TABLE IF NOT EXISTS " + tableName + " (" + fk1 + " INTEGER, " + fk2 + " INTEGER, PRIMARY KEY(" + fk1 + ", " + fk2 + "));";
        try { db->exec(sql); return true; } catch (...) { return false; }
    }

    template <typename T>
    bool save(T& entity) {
        DbTableMapper mapper; entity.inspect(mapper);
        try {
            SQLite::Statement query(*db, mapper.insertSQL());
            DbStatementBinder binder(query); entity.inspect(binder);
            query.exec(); 

            if (entity.id <= 0) {
                entity.id = static_cast<int>(db->getLastInsertRowid());
            }
            return true;
        } catch (std::exception& e) { 
            std::cerr << "Errore Salvataggio DB: " << e.what() << std::endl; 
            return false; 
        }
    }

    template <typename T>
    std::vector<T> getAll() {
        std::vector<T> list; DbTableMapper mapper; T dummy; dummy.inspect(mapper);
        try {
            SQLite::Statement query(*db, "SELECT * FROM " + mapper.tableName + ";");
            while (query.executeStep()) {
                T entity; DbRowExtractor extractor(query); entity.inspect(extractor); list.push_back(entity);
            }
        } catch (...) {}
        return list;
    }

    template <typename T>
    bool getById(int id, T& outEntity) {
        DbTableMapper mapper; T dummy; dummy.inspect(mapper);
        try {
            SQLite::Statement query(*db, "SELECT * FROM " + mapper.tableName + " WHERE id = ?;");
            query.bind(1, id);
            if (query.executeStep()) {
                DbRowExtractor extractor(query); outEntity.inspect(extractor); return true;
            }
        } catch (...) {}
        return false;
    }

    template <typename T>
    std::vector<T> getByField(const std::string& fieldName, int value) {
        std::vector<T> list; DbTableMapper mapper; T dummy; dummy.inspect(mapper);
        try {
            SQLite::Statement query(*db, "SELECT * FROM " + mapper.tableName + " WHERE " + fieldName + " = ?;");
            query.bind(1, value);
            while (query.executeStep()) {
                T entity; DbRowExtractor extractor(query); entity.inspect(extractor); list.push_back(entity);
            }
        } catch (...) {}
        return list;
    }

    template <typename TTarget>
    std::vector<TTarget> getManyToMany(const std::string& junctionTable, const std::string& sourceFk, const std::string& targetFk, int sourceId) {
        std::vector<TTarget> list; DbTableMapper targetMapper; TTarget dummy; dummy.inspect(targetMapper);
        try {
            std::string sql = "SELECT t.* FROM " + targetMapper.tableName + " t JOIN " + junctionTable + " j ON t.id = j." + targetFk + " WHERE j." + sourceFk + " = ?;";
            SQLite::Statement query(*db, sql);
            query.bind(1, sourceId);
            while (query.executeStep()) {
                TTarget entity; DbRowExtractor extractor(query); entity.inspect(extractor); list.push_back(entity);
            }
        } catch (...) {}
        return list;
    }

    bool linkManyToMany(const std::string& junctionTable, const std::string& fk1, int id1, const std::string& fk2, int id2) {
        try {
            SQLite::Statement query(*db, "INSERT OR IGNORE INTO " + junctionTable + " (" + fk1 + ", " + fk2 + ") VALUES (?, ?);");
            query.bind(1, id1); query.bind(2, id2); query.exec(); return true;
        } catch (...) { return false; }
    }

    bool unlinkManyToMany(const std::string& junctionTable, const std::string& fk1, int id1, const std::string& fk2, int id2) {
        try {
            SQLite::Statement query(*db, "DELETE FROM " + junctionTable + " WHERE " + fk1 + " = ? AND " + fk2 + " = ?;");
            query.bind(1, id1); query.bind(2, id2); query.exec(); return true;
        } catch (...) { return false; }
    }
};