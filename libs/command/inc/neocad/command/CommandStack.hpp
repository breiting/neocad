#pragma once
#include <vector>
#include <memory>
#include <neocad/command/ICommand.hpp>
#include <neocad/core/Logger.hpp>

namespace nc::cmd {

class CommandStack {
public:
    CommandStack(domain::Registry& reg, domain::GeometrySystem& geom) 
        : m_Registry(reg), m_Geometry(geom) {}

    void Push(std::unique_ptr<ICommand> cmd) {
        // If we are not at the tip, discard redo history
        if (m_Index < m_Stack.size()) {
            m_Stack.resize(m_Index);
        }

        cmd->Execute(m_Registry, m_Geometry);
        m_Stack.push_back(std::move(cmd));
        m_Index++;
    }

    void Undo() {
        if (m_Index > 0) {
            m_Stack[--m_Index]->Undo(m_Registry, m_Geometry);
        }
    }

    void Redo() {
        if (m_Index < m_Stack.size()) {
            m_Stack[m_Index]->Execute(m_Registry, m_Geometry);
            m_Index++;
        }
    }
    
    // Returns the index of the next command to be added (current tip)
    size_t GetCurrentIndex() const { return m_Index; }
    
    // Undo until we reach a specific index (useful for cancelling tool operations)
    void UndoTo(size_t targetIndex) {
        LOG(INFO) << "UndoTo: target=" << targetIndex << ", current=" << m_Index;
        while (m_Index > targetIndex) {
            Undo();
        }
    }

private:
    domain::Registry& m_Registry;
    domain::GeometrySystem& m_Geometry;
    
    std::vector<std::unique_ptr<ICommand>> m_Stack;
    size_t m_Index = 0; // Points to the slot for the NEXT command (past the last executed one)
};

} // namespace nc::cmd
