# Documento di Specifica dei Requisiti (SRS)

**Progetto:** DnDMaster
**Versione:** 0.1.0
**Architettura:** Client-Server Autorevole (Offline-First)

## 1. Introduzione
DnDMaster è un'applicazione desktop ibrida pensata per facilitare la gestione di campagne di giochi di ruolo cartacei (specificamente D&D). L'applicativo si divide in due macro-ambienti: il **Workspace** (modalità offline per la preparazione) e la **Session** (modalità online/LAN per il gioco dal vivo).

## 2. Tipologia di Utenti (Attori)
* **Master (DM):** Il creatore del mondo. Agisce come Host (Server) durante le sessioni live.
* **Player (PG):** Il giocatore. Agisce come Client durante le sessioni live, connettendosi all'host del Master.

## 3. Requisiti Funzionali

### 3.1 Modalità Master Offline (Workspace)
* **FR-M1 (Campagne):** Il Master deve poter creare, rinominare e caricare file di campagna (database locali).
* **FR-M2 (Map Editor):** Il Master deve poter disegnare mappe vettoriali (muri, pavimenti, ostacoli) divise per livelli/layer.
* **FR-M3 (NPC & Mostri):** Il Master deve poter creare schede per NPC e Mostri, salvandole nel database della campagna.
* **FR-M4 (Negozi):** Il Master deve poter creare inventari per i mercanti (Shops) prezzando gli oggetti.
* **FR-M5 (Note):** Il Master deve avere un text editor integrato per le note di sessione e la lore.

### 3.2 Modalità Player Offline (Workspace)
* **FR-P1 (Personaggi):** Il Player deve poter creare e gestire schede personaggio (Viewistiche, abilità).
* **FR-P2 (Inventario/Magie):** Il Player deve poter gestire il proprio inventario e libro degli incantesimi offline (es. durante un level-up a casa).
* **FR-P3 (Varianti):** Il Player deve poter creare cloni/varianti dello stesso personaggio per campagne diverse.

### 3.3 Modalità Sessione Live (Online)
* **FR-L1 (Handshake):** Alla connessione, il Client (Player) invia automaticamente l'ultima versione della sua scheda al Server (Master).
* **FR-L2 (Sync Mappa):** Il Master può rivelare zone della mappa e spostare i token; le modifiche appaiono live sui Client.
* **FR-L3 (Gestione Turni):** Il Master avvia l'Iniziativa. L'interfaccia di tutti i Client si aggiorna mostrando il turno corrente.
* **FR-L4 (Danni/Cure Dirette):** Il Master può infliggere danni o curare i player/NPC. Le schede locali dei player si aggiornano automaticamente.
* **FR-L5 (Buff/Debuff):** I giocatori o il Master possono applicare alterazioni di Stato a target specifici.
* **FR-L6 (Shop Condiviso):** Il Master può inviare l'interfaccia di uno Shop specifico a uno o più player per permettere acquisti live (con detrazione automatica delle monete).

## 4. Requisiti Non Funzionali
* **NFR-1 (Performance Grafica):** Il rendering vettoriale (NanoVG) e la UI (ImGui) devono mantenere i 60 FPS costanti.
* **NFR-2 (Rete):** Il protocollo di rete deve essere UDP Reliable (tramite ENet) per minimizzare la latenza.
* **NFR-3 (Dati):** I salvataggi devono essere file SQLite (con eventuale estensione proprietaria `.dndcamp` / `.dndchar`) per garantire integrità transazionale. L'Undo/Redo è implementato tramite Command Pattern.
* **NFR-4 (Portabilità):** Il codice deve compilare tramite CMake/vcpkg su Windows, macOS e Linux (Architettura C++20).