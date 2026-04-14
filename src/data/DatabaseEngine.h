#pragma once
#include <SQLiteCpp/SQLiteCpp.h>
#include <memory>
#include <iostream>
#include <vector>
#include <type_traits>
#include "IDatabaseEntity.h"

class DatabaseEngine {
public:
    DatabaseEngine() : db(nullptr) {}
    ~DatabaseEngine() { close(); }

    bool open(const std::string& path) {
        try {
            db = std::make_unique<SQLite::Database>(path, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
            return true;
        } catch (std::exception& e) {
            std::cerr << "Errore DB: " << e.what() << std::endl;
            return false;
        }
    }

    void close() { db.reset(); }

    // Genera la tabella chiedendo la query all'entità
    bool setupEntitySchema(const IDatabaseEntity& entity) {
        if (!db) return false;
        try {
            db->exec(entity.getCreateTableSQL());
            return true;
        } catch (std::exception& e) {
            std::cerr << "Errore Schema " << entity.getTableName() << ": " << e.what() << std::endl;
            return false;
        }
    }

    // Salva un record
    bool saveEntity(const IDatabaseEntity& entity) {
        if (!db) return false;
        try {
            SQLite::Statement query(*db, entity.getInsertSQL());
            entity.bindToStatement(query);
            query.exec();
            return true;
        } catch (std::exception& e) {
            std::cerr << "Errore saveEntity su " << entity.getTableName() << ": " << e.what() << std::endl;
            return false;
        }
    }

    // METODO GENERICO MAGICO: Estrae automaticamente tutti i record di qualsiasi classe Entità!
    template<typename T>
    std::vector<T> getAll() {
        static_assert(std::is_base_of<IDatabaseEntity, T>::value, "T deve ereditare da IDatabaseEntity");
        std::vector<T> list;
        if (!db) return list;
        try {
            T dummy; // Usiamo un'istanza vuota solo per farci dare il nome della tabella
            SQLite::Statement query(*db, "SELECT * FROM " + dummy.getTableName());
            while (query.executeStep()) {
                T entity;
                entity.loadFromRow(query);
                list.push_back(entity);
            }
        } catch (std::exception& e) {
            std::cerr << "Errore getAll: " << e.what() << std::endl;
        }
        return list;
    }

    template <typename... Args>
    bool executeCommand(const std::string& sql, const Args&... args) {
        if (!db) return false;
        try {
            SQLite::Statement query(*db, sql);
            int index = 1;
            // Magia C++17 (Fold Expression): binda tutti gli N argomenti automaticamente!
            ((query.bind(index++, args)), ...); 
            query.exec();
            return true;
        } catch (std::exception& e) {
            std::cerr << "Errore executeCommand: " << e.what() << std::endl;
            return false;
        }
    }

    template <typename T, typename... Args>
    std::vector<T> getCustom(const std::string& sql, const Args&... args) {
        static_assert(std::is_base_of<IDatabaseEntity, T>::value, "T deve ereditare da IDatabaseEntity");
        std::vector<T> list;
        if (!db) return list;
        try {
            SQLite::Statement query(*db, sql);
            int index = 1;
            ((query.bind(index++, args)), ...); 
            
            while (query.executeStep()) {
                T entity;
                entity.loadFromRow(query); // L'entità si riempie da sola!
                list.push_back(entity);
            }
        } catch (std::exception& e) {
            std::cerr << "Errore getCustom: " << e.what() << std::endl;
        }
        return list;
    }

    SQLite::Database& getConnection() { return *db; }

private:
    std::unique_ptr<SQLite::Database> db;
};