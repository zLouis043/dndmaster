#pragma once

// Forward declaration per evitare include circolari
class Engine;

class IAppView {
public:
    virtual ~IAppView() = default;
    
    // Chiamato una sola volta quando si "entra" in questa schermata
    virtual void onEnter(Engine* engine) = 0;
    
    // Chiamato ad ogni frame (qui metteremo la logica e ImGui)
    virtual void onUpdate(Engine* engine, float deltaTime) = 0;
    
    // Chiamato una sola volta quando si "esce" da questa schermata
    virtual void onExit(Engine* engine) = 0;
};