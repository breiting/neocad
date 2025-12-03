#pragma once
#include <vector>
#include <memory>
#include <ontoflow/command/ICommand.hpp>
#include <ontoflow/core/Logger.hpp> // For debugging logs

namespace of::cmd {

/**
 * \brief Manages a stack of commands, enabling undo and redo functionality.
 *
 * The CommandStack stores `ICommand` objects and provides an interface to
 * execute commands, undo them, redo them, and revert to a specific point
 * in the command history.
 */
class CommandStack {
public:
    /**
     * \brief Constructs a CommandStack.
     * \param reg Reference to the central ECS registry.
     * \param geom Reference to the geometry system.
     */
    CommandStack(domain::Registry& reg, domain::GeometrySystem& geom) 
        : m_Registry(reg), m_Geometry(geom) {}

    /**
     * \brief Pushes a new command onto the stack and executes it.
     *
     * If the current position in the history is not at the end, any
     * "redoable" commands are discarded.
     * \param cmd A unique pointer to the command to be executed and stored.
     */
    void Push(std::unique_ptr<ICommand> cmd) {
        // If we are not at the tip, discard redo history
        if (m_Index < m_Stack.size()) {
            m_Stack.resize(m_Index);
        }

        cmd->Execute(m_Registry, m_Geometry);
        m_Stack.push_back(std::move(cmd));
        m_Index++;
    }

    /**
     * \brief Undoes the last executed command on the stack.
     * Decrements the history index and calls `Undo()` on the command.
     */
    void Undo() {
        if (m_Index > 0) {
            m_Stack[--m_Index]->Undo(m_Registry, m_Geometry);
        }
    }

    /**
     * \brief Redoes the next command in the history.
     * Increments the history index and calls `Execute()` on the command.
     */
    void Redo() {
        if (m_Index < m_Stack.size()) {
            m_Stack[m_Index]->Execute(m_Registry, m_Geometry);
            m_Index++;
        }
    }
    
    /**
     * \brief Returns the current index in the command history.
     * This index points to the slot where the next command would be added.
     * Useful for marking a "transaction start" point for tools.
     * \return The current history index.
     */
    size_t GetCurrentIndex() const { return m_Index; }
    
    /**
     * \brief Undoes commands until a specific target index in the history is reached.
     * This is useful for cancelling multi-step tool operations.
     * \param targetIndex The index to undo to. Commands at and before this index will remain executed.
     */
    void UndoTo(size_t targetIndex) {
        LOG(Info) << "UndoTo: target=" << targetIndex << ", current=" << m_Index;
        while (m_Index > targetIndex) {
            Undo();
        }
    }

private:
    domain::Registry& m_Registry; ///< Reference to the ECS registry.
    domain::GeometrySystem& m_Geometry; ///< Reference to the geometry system.
    
    std::vector<std::unique_ptr<ICommand>> m_Stack; ///< The actual stack of commands.
    size_t m_Index = 0; ///< Current position in the command history (points to next command slot).
};

} // namespace of::cmd
