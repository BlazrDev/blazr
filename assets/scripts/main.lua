-- Main Script
--
gEntity = Entity("E1", "G1")
local transform = TransformComponent(100, 100, 1, 1, 10)
gEntity:add_component(transform)
-- -- -- gEntity:add_component(SpriteComponent(200, 200, "chammy", 0, 0, 0))
-- --
gEntity2 = Entity("E2", "G1")
local transform2 = gEntity2:add_component(TransformComponent(100, 300, 1, 1, 10))
local sprite2 = gEntity2:add_component(SpriteComponent(200, 200, "masha", 0, 0, 0))

local view = Registry.get_entities(TransformComponent)

view:exclude(SpriteComponent)

view:for_each(function(entity)
    print(entity:name())
end)
-- local transoform = gEntity:add_component(TransformComponent(100, 100, 0, 0, 0))
-- print(transoform.scale)
-- local x = 0.0
-- local max_x = 800.0
--
-- gEntity2:remove_component(TransformComponent)
-- local s1 = gEntity:has_component(TransformComponent)
-- print(s1)

main = {
    [1] = {
        update = function()
            -- if x < max_x then
            --     transform:set_position(x, 300.0)
            --     x = x + 20
            -- end
            --
            -- if x >= max_x then
            --     x = 0
            -- end

            -- local s1 = gEntity2:remove_component(sprite2)
            -- print(s1.width)
        end,
    },
    [2] = {
        render = function()
            -- print("We are rendering in lua!")
        end,
    },
}
