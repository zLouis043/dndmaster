#pragma once
#include "ISerializable.h"
#include "IDatabaseEntity.h"
#include "QueryBuilder.h"
#include <SQLiteCpp/SQLiteCpp.h>

class MapEntity : public ISerializable, public IDatabaseEntity {
public:
    int id = -1;
    int campaignId = 1;
    std::string name = "Nuova Mappa";
    std::string jsonVectorData = "{}";
    int gridSize = 50;

    std::string serialize() const override { return "{}"; }
    void deserialize(const std::string& data) override {}

    std::string getTableName() const override { return "MAP"; }
    
    std::string getCreateTableSQL() const override {
        return QueryBuilder::createTable(getTableName())
            .column("id", "INTEGER PRIMARY KEY")
            .column("campaign_id", "INTEGER")
            .column("name", "TEXT")
            .column("json_vector_data", "TEXT")
            .column("grid_size", "INTEGER")
            .build();
    }

    std::string getInsertSQL() const override {
        return QueryBuilder::insertInto(getTableName())
            .column("campaign_id")
            .column("name")
            .column("json_vector_data")
            .column("grid_size")
            .build();
    }

    void bindToStatement(SQLite::Statement& query) const override {
        query.bind(1, campaignId);
        query.bind(2, name);
        query.bind(3, jsonVectorData);
        query.bind(4, gridSize);
    }

    void loadFromRow(SQLite::Statement& query) override {
        id = query.getColumn(0).getInt();
        campaignId = query.getColumn(1).getInt();
        name = query.getColumn(2).getText();
        jsonVectorData = query.getColumn(3).getText();
        gridSize = query.getColumn(4).getInt();
    }
};