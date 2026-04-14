#pragma once
#include <string>

namespace SQLite { class Statement; }

class IDatabaseEntity {
public:
    virtual ~IDatabaseEntity() = default;

    virtual std::string getTableName() const = 0;
    virtual std::string getCreateTableSQL() const = 0; 
    virtual std::string getInsertSQL() const = 0;
    
    virtual void bindToStatement(SQLite::Statement& query) const = 0;
    virtual void loadFromRow(SQLite::Statement& query) = 0;
};