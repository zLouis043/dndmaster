# Schema del Database (Entity-Relationship Diagram)

L'applicazione utilizza SQLiteCpp per gestire i dati. 
La separazione fisica dei file (`.dndcamp` per il Master, `.dndchar` per il Player) garantisce la portabilità e la modalità offline.

## Database Campagna del Master (`.dndcamp`)

```mermaid
erDiagram
    CAMPAIGN {
        int id PK
        string name
        string lore_notes
        int current_map_id FK
    }
    
    MAP {
        int id PK
        int campaign_id FK
        string name
        string json_vector_data "Tracciati NanoVG"
        int grid_size
    }
    
    NPC {
        int id PK
        int campaign_id FK
        string name
        int hp
        string stats_json
    }
    
    SHOP {
        int id PK
        int campaign_id FK
        string name
        string owner_name
    }
    
    SHOP_ITEM {
        int id PK
        int shop_id FK
        string item_name
        int cost_copper
    }
    
    PLAYER_ROSTER {
        int id PK
        int campaign_id FK
        string player_name
        string latest_char_json "Cache della scheda inviata al login"
    }

    CAMPAIGN ||--o{ MAP : contains
    CAMPAIGN ||--o{ NPC : contains
    CAMPAIGN ||--o{ SHOP : contains
    CAMPAIGN ||--o{ PLAYER_ROSTER : tracks
    SHOP ||--o{ SHOP_ITEM : sells
```

## Database Personaggio del Player (`.dndchar`)

```mermaid
erDiagram
    CHARACTER {
        int id PK
        string name
        string race
        string class
        int level
        int current_hp
        int max_hp
        int copper_coins
    }
    
    INVENTORY {
        int id PK
        int character_id FK
        string item_name
        string description
        int quantity
        boolean is_equipped
    }
    
    SPELLBOOK {
        int id PK
        int character_id FK
        int spell_level
        string spell_name
        string description
    }

    CHARACTER ||--o{ INVENTORY : holds
    CHARACTER ||--o{ SPELLBOOK : knows
```