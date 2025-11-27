#pragma once
#include <neocad/command/ICommand.hpp>
#include <neocad/domain/Components.hpp>
#include <neocad/core/Logger.hpp>

namespace nc::cmd {

class CreatePointCommand : public ICommand {
public:
    CreatePointCommand(const glm::vec3& pos) : m_Pos(pos) {}

    void Execute(domain::Registry& registry, domain::GeometrySystem& geom) override {
        if (m_Entity == domain::INVALID_ENTITY) {
            // First run: Create fresh
            m_Entity = geom.CreatePoint(m_Pos);
        } else {
            // Redo: Re-create or re-add components using the SAME ID if possible, 
            // but our Registry::CreateEntity increments ID. 
            // Ideally Registry allows forcing ID, or we just resurrect components.
            // For MVP: We just re-create components on the zombie entity if it's still valid,
            // OR we assume the Registry doesn't delete slots, just components.
            
            // Let's check if entity is "valid" in registry terms. 
            // If the Registry removed it entirely, we need a new one, 
            // BUT that breaks references (Topology).
            // FIX: We assume the Entity ID is stable and just add components back.
             registry.AddComponent(m_Entity, domain::PositionComponent{m_Pos});
        }
    }

    void Undo(domain::Registry& registry, domain::GeometrySystem& /*geom*/) override {
        // "Soft delete" -> remove components
        if (m_Entity != domain::INVALID_ENTITY) {
            registry.RemoveComponent<domain::PositionComponent>(m_Entity);
        }
    }
    
    domain::Entity GetEntity() const { return m_Entity; }

private:
    glm::vec3 m_Pos;
    domain::Entity m_Entity{domain::INVALID_ENTITY};
};

class CreateLineCommand : public ICommand {
public:
    CreateLineCommand(domain::Entity p0, domain::Entity p1) : m_P0(p0), m_P1(p1) {}

    void Execute(domain::Registry& registry, domain::GeometrySystem& geom) override {
        if (m_Entity == domain::INVALID_ENTITY) {
             m_Entity = geom.CreateLine(m_P0, m_P1);
        } else {
             registry.AddComponent(m_Entity, domain::EdgeComponent{m_P0, m_P1});
        }
    }

    void Undo(domain::Registry& registry, domain::GeometrySystem& /*geom*/) override {
        if (m_Entity != domain::INVALID_ENTITY) {
            registry.RemoveComponent<domain::EdgeComponent>(m_Entity);
        }
    }

private:
    domain::Entity m_P0;
    domain::Entity m_P1;
    domain::Entity m_Entity{domain::INVALID_ENTITY};
};

template <typename T>
class CreateComponentCommand : public ICommand {
   public:
    CreateComponentCommand(domain::Registry& /*reg*/, domain::Entity e, T comp, bool newEntity = false)
        : m_Entity(e), m_Component(comp), m_CreatedEntity(newEntity) {
    }

    void Execute(domain::Registry& registry, domain::GeometrySystem& /*geom*/) override {
        // We just forcefully add the component, assuming m_Entity is a valid ID managed by the caller/registry logic.
        registry.AddComponent<T>(m_Entity, m_Component);
    }

    void Undo(domain::Registry& registry, domain::GeometrySystem& /*geom*/) override {
        LOG(INFO) << "Undo CreateComponent: Removing component from Entity " << m_Entity;
        registry.RemoveComponent<T>(m_Entity);
        // Note: If m_CreatedEntity is true, we leave the "shell" entity in registry.
        // A proper system would mark it dead or use a CreateEntityCommand separately.
    }

   private:
    domain::Entity m_Entity;
    T m_Component;
    bool m_CreatedEntity;
};

} // namespace nc::cmd
