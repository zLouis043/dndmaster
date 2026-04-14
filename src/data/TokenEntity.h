#pragma once
#include "ISerializable.h"
#include "IDatabaseEntity.h"
#include "QueryBuilder.h"
#include "NpcEntity.h"
#include <SQLiteCpp/SQLiteCpp.h>

class TokenEntity : public ISerializable, public IDatabaseEntity {
public:
    int id = -1;
    int mapId = 1;
    int characterId = -1;
    float x = 0.0f;
    float y = 0.0f;
    
    // Per il rendering (si unisce tramite JOIN)
    NpcEntity charData; 

    // I token non hanno bisogno di JSON per ora
    std::string serialize() const override { return "{}"; }
    void deserialize(const std::string& data) override {}

    std::string getTableName() const override { return "MAP_TOKEN"; }
    
    std::string getCreateTableSQL() const override {
        return QueryBuilder::createTable(getTableName())
            .column("id", "INTEGER PRIMARY KEY")
            .column("map_id", "INTEGER")
            .column("character_id", "INTEGER")
            .column("pos_x", "REAL")
            .column("pos_y", "REAL")
            .build();
    }

    std::string getInsertSQL() const override {
        return QueryBuilder::insertInto(getTableName())
            .column("map_id")
            .column("character_id")
            .column("pos_x")
            .column("pos_y")
            .build();
    }

    void bindToStatement(SQLite::Statement& query) const override {
        query.bind(1, mapId);
        query.bind(2, characterId);
        query.bind(3, x);
        query.bind(4, y);
    }

    void loadFromRow(SQLite::Statement& query) override {
        id = query.getColumn(0).getInt();
        mapId = query.getColumn(1).getInt();
        characterId = query.getColumn(2).getInt();
        x = (float)query.getColumn(3).getDouble();
        y = (float)query.getColumn(4).getDouble();
        
        // Se la query include i campi aggiuntivi della JOIN (nome, hp, stats)
        if (query.getColumnCount() > 5) {
            charData.name = query.getColumn(5).getText();
            charData.hp = query.getColumn(6).getInt();
            charData.deserialize(query.getColumn(7).getText());
        }
    }
};