#pragma once
#include "ISerializable.h"
#include "IDatabaseEntity.h"
#include "QueryBuilder.h"
#include <nlohmann/json.hpp>
#include <SQLiteCpp/SQLiteCpp.h>

struct CharacterStats {
    int str = 10, dex = 10, con = 10;
    int intl = 10, wis = 10, cha = 10;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CharacterStats, str, dex, con, intl, wis, cha)

class NpcEntity : public ISerializable, public IDatabaseEntity {
public:
    int id = -1;
    std::string name = "Nuovo Eroe";
    int hp = 10;
    CharacterStats stats;

    // --- INTERFACCIA JSON ---
    std::string serialize() const override {
        nlohmann::json j = stats;
        return j.dump();
    }

    void deserialize(const std::string& data) override {
        if (!data.empty() && data != "{}") {
            try { stats = nlohmann::json::parse(data).get<CharacterStats>(); } 
            catch(...) {}
        }
    }

    // --- INTERFACCIA DATABASE ---
    std::string getTableName() const override { return "NPC"; }
    
    std::string getCreateTableSQL() const override {
        return QueryBuilder::createTable(getTableName())
            .column("id", "INTEGER PRIMARY KEY")
            .column("name", "TEXT")
            .column("hp", "INTEGER")
            .column("stats_json", "TEXT")
            .build();
    }

    std::string getInsertSQL() const override {
        return QueryBuilder::insertInto(getTableName())
            .column("name")
            .column("hp")
            .column("stats_json")
            .build();
    }

    void bindToStatement(SQLite::Statement& query) const override {
        query.bind(1, name);
        query.bind(2, hp);
        query.bind(3, serialize()); 
    }

    void loadFromRow(SQLite::Statement& query) override {
        id = query.getColumn(0).getInt();
        name = query.getColumn(1).getText();
        hp = query.getColumn(2).getInt();
        deserialize(query.getColumn(3).getText());
    }
};