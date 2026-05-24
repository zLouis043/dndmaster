#pragma once
#include <memory>
#include <vector>
#include <type_traits> 

class ICommand {
public:
    virtual ~ICommand() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
};

class CommandManager {
public:
    template <typename T, typename... Args>
    void execute(Args&&... args) {
        static_assert(std::is_base_of<ICommand, T>::value, "Il tipo deve ereditare da ICommand!");

        auto command = std::make_unique<T>(std::forward<Args>(args)...);
        
        command->execute();
        m_undoStack.push_back(std::move(command));
        m_redoStack.clear(); 
    }

    void undo() {
        if (m_undoStack.empty()) return;
        auto cmd = std::move(m_undoStack.back());
        m_undoStack.pop_back();
        cmd->undo();
        m_redoStack.push_back(std::move(cmd));
    }

    void redo() {
        if (m_redoStack.empty()) return;
        auto cmd = std::move(m_redoStack.back());
        m_redoStack.pop_back();
        cmd->execute();
        m_undoStack.push_back(std::move(cmd));
    }

    void clear() {
        m_undoStack.clear();
        m_redoStack.clear();
    }

private:
    std::vector<std::unique_ptr<ICommand>> m_undoStack;
    std::vector<std::unique_ptr<ICommand>> m_redoStack;
};