-- Entity API provided by the engine
Entity = {}
Entity.__index = Entity

function Entity:new(entity_id)
    local obj = { id = entity_id }
    setmetatable(obj, self)
    return obj
end

function Entity:get_component(component_name)
    -- This should call into the engine's C++ code to fetch the component
    return Engine.get_component(self.id, component_name)
end

function Entity:destroy()
    -- Call the engine to destroy this entity
    Engine.destroy_entity(self.id)
end
