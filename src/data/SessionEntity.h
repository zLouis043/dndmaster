#pragma once
#include "ISerializable.h"
#include "IDatabaseEntity.h"
#include "QueryBuilder.h"
#include <SQLiteCpp/SQLiteCpp.h>

class SessionEntity : public ISerializable, public IDatabaseEntity {
public:
    int id = -1;
    int campaignId = 1;
    std::string title = "Nuova Sessione";
    std::string date = "";
    std::string notes = "";
    bool isActive = false;

    std::string serialize() const override { return "{}"; }
    void deserialize(const std::string& data) override {}

    std::string getTableName() const override { return "SESSION"; }
    
    std::string getCreateTableSQL() const override {
        return QueryBuilder::createTable(getTableName())
            .column("id", "INTEGER PRIMARY KEY")
            .column("campaign_id", "INTEGER")
            .column("title", "TEXT")
            .column("date", "TEXT")
            .column("notes", "TEXT")
            .column("is_active", "BOOLEAN")
            .build();
    }

    std::string getInsertSQL() const override {
        return QueryBuilder::insertInto(getTableName())
            .column("campaign_id")
            .column("title")
            .column("date")
            .column("notes")
            .column("is_active")
            .build();
    }

    void bindToStatement(SQLite::Statement& query) const override {
        query.bind(1, campaignId);
        query.bind(2, title);
        query.bind(3, date);
        query.bind(4, notes);
        query.bind(5, isActive);
    }

    void loadFromRow(SQLite::Statement& query) override {
        id = query.getColumn(0).getInt();
        campaignId = query.getColumn(1).getInt();
        title = query.getColumn(2).getText();
        date = query.getColumn(3).getText();
        notes = query.getColumn(4).getText();
        isActive = query.getColumn(5).getInt() != 0;
    }
};