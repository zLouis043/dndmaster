# Architettura del Software e Class Diagram

Il software utilizza un'architettura modulare. La UI è staccata dai Dati, che a loro volta sono staccati dalla Rete. La comunicazione avviene tramite il **Command Pattern**.

## Diagramma delle Classi Principali

```mermaid
classDiagram
    class Engine {
        -Window window
        -Renderer renderer
        -UIManager ui
        -IAppView currentView
        +init() bool
        +run() void
        +changeView(IAppView newView)
    }

    class IAppView {
        <<interface>>
        +onEnter()
        +onUpdate(dt)
        +onExit()
    }

    class DatabaseManager {
        -SQLite::Database db
        +loadCampaign(path)
        +loadCharacter(path)
        +executeCommand(ICommand cmd)
    }

    class NetworkManager {
        -ENetHost* host
        +startServer(port)
        +connectTo(ip, port)
        +broadcastCommand(ICommand cmd)
        +sendCommand(peer, ICommand cmd)
    }

    class ICommand {
        <<interface>>
        +execute()
        +undo()
        +serialize() string
    }

    Engine "1" *-- "1" IAppView
    IAppView <|-- ViewMainMenu
    IAppView <|-- ViewMasterWorkspace
    IAppView <|-- ViewPlayerWorkspace
    IAppView <|-- ViewLiveSession

    ViewLiveSession "1" *-- "1" NetworkManager
    ViewLiveSession "1" *-- "1" DatabaseManager
    DatabaseManager ..> ICommand : esegue
    NetworkManager ..> ICommand : trasmette
```

## Macchina a Viewi (Ciclo di Vita dell'App)

```mermaid
ViewDiagram-v2
    [*] --> InitEngine
    InitEngine --> ViewMainMenu : SDL/ImGui Pronti

    ViewMainMenu --> ViewMasterWorkspace : Crea/Carica Campagna (Offline)
    ViewMainMenu --> ViewPlayerWorkspace : Modifica PG (Offline)
    ViewMainMenu --> ViewLiveSession : Host / Join

    ViewMasterWorkspace --> ViewMainMenu : Salva & Esci
    ViewPlayerWorkspace --> ViewMainMenu : Salva & Esci
    
    View ViewLiveSession {
        [*] --> Handshake
        Handshake --> SyncMappa E Roster
        SyncMappa E Roster --> InGioco
        InGioco --> SincronizzazioneComandi
    }

    ViewLiveSession --> ViewMainMenu : Disconnessione
```