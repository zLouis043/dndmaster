#include "ViewCampaignHub.h"
#include "ViewMainMenu.h"
#include "ViewSessionDashboard.h"
#include "ViewCharacterEditor.h"
#include "ViewLevelEditor.h"
#include "../../core/engine.h"

void ViewCampaignHub::onEnter() {
    // --- NAVIGAZIONE TABS ---
    auto switchTab = [this](const std::string& activePanel, const std::string& activeBtn) {
        for (auto p : {"panel_sessions", "panel_characters", "panel_maps"}) 
            if (auto el = getDocument()->GetElementById(p)) el->SetProperty("display", "none");
        for (auto b : {"tab_btn_sessions", "tab_btn_characters", "tab_btn_maps"}) 
            if (auto el = getDocument()->GetElementById(b)) el->SetClass("active", false);

        if (auto el = getDocument()->GetElementById(activePanel)) el->SetProperty("display", "block");
        if (auto el = getDocument()->GetElementById(activeBtn)) el->SetClass("active", true);
    };

    bindEvent("tab_btn_sessions", Rml::EventId::Click, [=](Rml::Event&) { switchTab("panel_sessions", "tab_btn_sessions"); });
    bindEvent("tab_btn_characters", Rml::EventId::Click, [=](Rml::Event&) { switchTab("panel_characters", "tab_btn_characters"); });
    bindEvent("tab_btn_maps", Rml::EventId::Click, [=](Rml::Event&) { switchTab("panel_maps", "tab_btn_maps"); });
    bindEvent("btn_back_menu", Rml::EventId::Click, [this](Rml::Event&) { getEngine()->changeView<ViewMainMenu>(); });

    // --- AZIONI GLOBALI ---
    bindEvent("btn_new_session", Rml::EventId::Click, [this](Rml::Event&) {
        SessionEntity newSession; newSession.title = "Sessione " + std::to_string(allSessions.size() + 1);
        getEngine()->getDB().save(newSession); refreshData();
    });

    bindEvent("btn_new_character", Rml::EventId::Click, [this](Rml::Event&) { getEngine()->changeView<ViewCharacterEditor>(); });

    bindEvent("btn_new_map", Rml::EventId::Click, [this](Rml::Event&) {
        MapEntity m; m.name = "Mappa " + std::to_string(allMaps.size() + 1);
        getEngine()->getDB().save(m); refreshData();
    });

    bindEvent("btn_enter_session", Rml::EventId::Click, [this](Rml::Event&) {
        if (selectedSessionId != -1) getEngine()->changeView<ViewSessionDashboard>(selectedSessionId);
    });

    // --- DEFINIZIONE LISTE DINAMICHE TRAMITE DYINSPECTABLE ---
    m_dySessions.define([this](IInspector& ins, SessionEntity& sess) {
        std::function<void()> onClick = [this, id = sess.id]() { defer([this, id]() { selectSession(id); }); };
        std::string cls = (selectedSessionId == sess.id) ? "list-item selected" : "list-item";
        ins.property("btn_sess", onClick).tag("ui-label", sess.title).tag("ui-class", cls);
    });

    m_dyCharacters.define([this](IInspector& ins, NpcEntity& npc) {
        std::function<void()> onClick = [this, id = npc.id]() { getEngine()->changeView<ViewCharacterEditor>(id); };
        ins.property("btn_npc", onClick).tag("ui-label", npc.name + " (HP: " + std::to_string(npc.hp) + ")").tag("ui-class", "list-item");
    });

    m_dyMaps.define([this](IInspector& ins, MapEntity& map) {
        std::function<void()> onClick = [this, id = map.id]() { getEngine()->changeView<ViewLevelEditor>(id); };
        ins.property("btn_map", onClick).tag("ui-label", map.name).tag("ui-class", "list-item");
    });

    m_dyRosterIn.define([this](IInspector& ins, NpcEntity& npc) {
        std::function<void()> onClick = [this, id = npc.id]() { defer([this, id]() { toggleRoster(id, false); }); };
        ins.property("btn_in", onClick).tag("ui-label", "- " + npc.name).tag("ui-class", "list-item in-roster");
    });

    m_dyRosterOut.define([this](IInspector& ins, NpcEntity& npc) {
        std::function<void()> onClick = [this, id = npc.id]() { defer([this, id]() { toggleRoster(id, true); }); };
        ins.property("btn_out", onClick).tag("ui-label", "+ " + npc.name).tag("ui-class", "list-item out-roster");
    });

    // Colleghiamo i vettori agli adattatori UI
    m_dySessions.bind(allSessions);
    m_dyCharacters.bind(allNPCs);
    m_dyMaps.bind(allMaps);
    m_dyRosterIn.bind(sessionNPCs);
    m_dyRosterOut.bind(rosterOutNPCs); 

    refreshData();
}

void ViewCampaignHub::onExit() {
    unmount("mount_sessions"); unmount("mount_characters"); unmount("mount_maps");
    unmount("mount_roster_in"); unmount("mount_roster_out");
}

void ViewCampaignHub::refreshData() {
    if (!getDocument() || !getEngine()) return;

    allSessions = getEngine()->getDB().getAll<SessionEntity>();
    if (!allSessions.empty() && selectedSessionId == -1) selectedSessionId = allSessions[0].id;
    
    allNPCs = getEngine()->getDB().getAll<NpcEntity>();
    allMaps = getEngine()->getDB().getAll<MapEntity>();

    mount("mount_sessions", &m_dySessions, "list_sessions");
    mount("mount_characters", &m_dyCharacters, "list_characters");
    mount("mount_maps", &m_dyMaps, "list_maps");

    selectSession(selectedSessionId);
}

void ViewCampaignHub::selectSession(int id) {
    selectedSessionId = id;
    auto panel = getDocument()->GetElementById("session_details");
    if (!panel) return;

    if (id == -1) {
        panel->SetProperty("display", "none");
        return;
    }
    panel->SetProperty("display", "block");

    SessionEntity currentSession;
    getEngine()->getDB().getById(id, currentSession);

    if (auto title = getDocument()->GetElementById("lbl_session_title")) {
        title->SetInnerRML(currentSession.title);
    }

    // ORM in azione!
    sessionNPCs = getEngine()->getDB().getManyToMany<NpcEntity>("SESSION_NPC", "session_id", "npc_id", id);

    rosterOutNPCs.clear();
    for (const auto& npc : allNPCs) {
        auto it = std::find_if(sessionNPCs.begin(), sessionNPCs.end(), [&](const NpcEntity& n) { return n.id == npc.id; });
        if (it == sessionNPCs.end()) rosterOutNPCs.push_back(npc);
    }

    mount("mount_sessions", &m_dySessions, "list_sessions"); 
    mount("mount_roster_in", &m_dyRosterIn, "list_roster_in");
    mount("mount_roster_out", &m_dyRosterOut, "list_roster_out");
}

void ViewCampaignHub::toggleRoster(int npcId, bool addToSession) {
    if (selectedSessionId == -1) return;
    if (addToSession) getEngine()->getDB().linkManyToMany("SESSION_NPC", "session_id", selectedSessionId, "npc_id", npcId);
    else getEngine()->getDB().unlinkManyToMany("SESSION_NPC", "session_id", selectedSessionId, "npc_id", npcId);
    selectSession(selectedSessionId); 
}