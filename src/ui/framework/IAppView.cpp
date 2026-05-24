#include "IAppView.h"
#include "../../core/engine.h"
#include "RmlUiAutoInspector.h" 
#include "../../data/inspector/Inspection.h"

void IAppView::enter(Engine* engine) {
    m_engine = engine;

    m_engine->getShortcuts().clearLocal(); 
    m_engine->getEvents().clearLocal();
    m_engine->getCommands().clear();

    if (!m_rmlPath.empty()) {
        std::string fullPath = m_engine->getAssetPath() + m_rmlPath;
        document = m_engine->getUIContext()->LoadDocument(fullPath);
        
        if (document) {
            document->Show();
        } else {
            std::cerr << "[ERRORE] Framework: Impossibile caricare la view: " << fullPath << std::endl;
        }
    }
    onEnter();
}

void IAppView::update(float deltaTime) {
    if (!m_deferredActions.empty()) {
        auto actions = m_deferredActions;
        m_deferredActions.clear();
        for (auto& a : actions) a();
    }

    if (document) {
        for (auto& [id, info] : m_mounts) {
            if (info.obj) {
                auto container = document->GetElementById(info.defaultContainerId);
                if (container) {
                    RmlUiAutoInspector reader(document, container, RmlUiAutoInspector::Mode::READ);
                    info.obj->inspect(reader);
                }
            }
        }
    }
    onUpdate(deltaTime);
}

void IAppView::exit() {
    onExit();

    if (document) {
        for (auto& [id, info] : m_mounts) {
            clearUI(info.relatedTargets);
            clearUI({info.defaultContainerId});
        }
    }
    m_mounts.clear();

    if (document && m_engine && m_engine->getUIContext()) {
        Rml::ElementList canvases;
        document->GetElementsByTagName(canvases, "skia-canvas");
        
        for (auto el : canvases) {
            if (auto canvasEl = dynamic_cast<ElementSkiaCanvas*>(el)) {
                canvasEl->SetDrawCallback(nullptr); 
            }
        }

        m_engine->getUIContext()->UnloadDocument(document);
        document = nullptr;
    }
}

void IAppView::bindEvent(const std::string& element_id, Rml::EventId event_id, std::function<void(Rml::Event&)> callback) {
    if (!document) return;
    auto element = document->GetElementById(element_id);
    if (element) {
        element->AddEventListener(event_id, new LambdaListener(std::move(callback)));
    } else {
        std::cerr << "[WARN] Impossibile fare il bind: Elemento '" << element_id << "' non trovato nel DOM!" << std::endl;
    }
}

void IAppView::bindCanvas(const std::string& element_id, std::function<void(class SkCanvas*, float, float)> drawCallback) {
    if (!document) return;
    auto el = document->GetElementById(element_id);
    if (el) {
        if (auto canvasEl = dynamic_cast<ElementSkiaCanvas*>(el)) {
            canvasEl->SetDrawCallback(std::move(drawCallback));
        } else {
            std::cerr << "[WARN] Impossibile usare bindCanvas: L'elemento '" << element_id << "' non e' un <skia-canvas>!" << std::endl;
        }
    }
}

void IAppView::addShortcut(KeyCode key, KeyModifiers mods, std::function<void()> action) {
    m_engine->getShortcuts().addLocal(key, mods, std::move(action));
}

void IAppView::mount(const std::string& mountId, IInspectable* obj, const std::string& defaultContainerId, const std::vector<std::string>& relatedTargets) {
    if (m_mounts.count(mountId)) {
        auto& old = m_mounts[mountId];
        clearUI(old.relatedTargets);
        clearUI({old.defaultContainerId});
    }

    if (!obj) {
        m_mounts.erase(mountId);
        return;
    }

    m_mounts[mountId] = { obj, defaultContainerId, relatedTargets };
    clearUI(relatedTargets);
    clearUI({defaultContainerId});
    buildUI(obj, defaultContainerId);
}

void IAppView::unmount(const std::string& mountId) {
    mount(mountId, nullptr, "");
}

void IAppView::readUI(IInspectable* obj, const std::string& defaultContainerId) {
    if (!document || !obj) return;
    auto container = document->GetElementById(defaultContainerId);
    if (container) {
        RmlUiAutoInspector reader(document, container, RmlUiAutoInspector::Mode::READ);
        obj->inspect(reader);
    }
}

void IAppView::writeUI(IInspectable* obj, const std::string& defaultContainerId) {
    if (!document || !obj) return;
    auto container = document->GetElementById(defaultContainerId);
    if (container) {
        RmlUiAutoInspector writer(document, container, RmlUiAutoInspector::Mode::WRITE);
        obj->inspect(writer);
    }
}

void IAppView::clearUI(const std::vector<std::string>& containerIds) {
    if (!document) return;
    for (const auto& id : containerIds) {
        if (auto el = document->GetElementById(id)) el->SetInnerRML("");
    }
}

void IAppView::buildUI(IInspectable* obj, const std::string& defaultContainerId) {
    auto container = document->GetElementById(defaultContainerId);
    if (!container) return;

    RmlUiAutoInspector generator(document, container, RmlUiAutoInspector::Mode::GENERATE);
    obj->inspect(generator);

    RmlUiAutoInspector writer(document, container, RmlUiAutoInspector::Mode::WRITE);
    obj->inspect(writer);

    RmlUiAutoInspector reader(document, container, RmlUiAutoInspector::Mode::READ);
    obj->inspect(reader);
}