# Protocolli di Rete e Diagrammi di Sequenza

L'architettura di rete è **Server-Authoritative**. I client non modificano mai lo Stato globale del gioco direttamente, ma inviano richieste sotto forma di *Comandi JSON* che il Master valida.

## 1. Flowchart / BPMN: L'Handshake di Connessione
Quando un Player si connette, deve inviare la sua scheda personaggio aggiornata (livellata offline).

```mermaid
sequenceDiagram
    participant Player (Client)
    participant Master (Server)
    participant DB Master (SQLite)

    Player->>Master: Connect(IP, Porta)
    Master-->>Player: Connection Accepted
    Note over Player: Serializza il file .dndchar<br/>in JSON
    Player->>Master: [Pacchetto Handshake] JSON Personaggio
    Note over Master: Valida i dati del personaggio
    Master->>DB Master: UPDATE roster_players SET data = JSON
    Master-->>Player: [Pacchetto Sync] JSON Mappa Corrente + NPC
    Note over Player: Renderizza la mappa ricevuta
```

## 2. BPMN / Flowchart: Combattimento (Il Master infligge danno)
Flusso logico di quando un Master compie un'azione diretta su un giocatore.

```mermaid
sequenceDiagram
    participant Master UI
    participant Master DB
    participant Network Server
    participant Player UI
    participant Player DB

    Note over Master UI: Master seleziona "Palla di Fuoco"<br/>Target: Giocatore 1
    Master UI->>Master DB: Applica Danno (Registra Log)
    Master UI->>Network Server: Crea CmdApplyDamage(PG1, 20)
    Network Server->>Player UI: Trasmetti CmdApplyDamage in JSON
    Note over Player UI: Riceve Comando
    Player UI->>Player DB: UPDATE HP = HP - 20
    Note over Player UI: Effetto visivo schermo rosso<br/>Animazione "-20"
```

## 3. Flusso di un'azione del Player (Es. Acquisto allo Shop)
Il Player tenta un'azione. Il Master deve validarla prima che diventi effettiva per tutti.

```mermaid
sequenceDiagram
    participant Player
    participant Master
    
    Player->>Player: Clicca su "Compra Pozione (50mo)"
    Player->>Master: Invia CmdBuyItem(Pozione, 50mo)
    Note over Master: Controlla se il Player<br/>ha abbastanza monete
    alt Transazione Valida
        Master->>Master: Scala monete, aggiunge oggetto al DB Master
        Master-->>Player: Broadcast CmdBuyItemApproved
        Note over Player: Scala le monete dal DB locale<br/>e mette in inventario
    else Transazione Non Valida
        Master-->>Player: Broadcast CmdError("Fondi Insufficienti")
    end
```