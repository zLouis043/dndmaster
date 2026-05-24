#pragma once

#include <vector>
#include <functional>

class DatabaseEngine;

class DbSchemaRegistry {
public:
    using RegFunc = std::function<void(DatabaseEngine&)>;
    static void registerSchema(RegFunc f) { getRegistry().push_back(f); }
    static const std::vector<RegFunc>& getSchemas() { return getRegistry(); }
private:
    static std::vector<RegFunc>& getRegistry() { static std::vector<RegFunc> r; return r; }
};

#define REGISTER_DB_ENTITY(CLASS_NAME) \
    static const bool reg_db_##CLASS_NAME = []() { \
        DbSchemaRegistry::registerSchema([](DatabaseEngine& db) { \
            db.registerSchema<CLASS_NAME>(); \
        }); \
        return true; \
    }();