-- Main Script
--
gEntity = Entity("E1", "G1")
gEntity:add_component(TransformComponent(100, 100, 1, 1, 0))
gEntity:add_component(SpriteComponent(200, 200, "chammy", 0, 0, 0))

gEntity = Entity("E1", "G1")
local transform = gEntity:add_component(TransformComponent(100, 300, 1, 1, 0))
local sprite = gEntity:add_component(SpriteComponent(200, 200, "masha", 0, 0, 0))

-- local transoform = gEntity:add_component(TransformComponent(100, 100, 0, 0, 0))
-- print(transoform.scale)
local x = 0.0
local max_x = 800.0
main = {
    [1] = {
        update = function()
            if x < max_x then
                transform:set_position(x, 300.0)
                x = x + 20
            end

            if x >= max_x then
                x = 0
            end
            -- print("We are updating in lua!")
        end,
    },
    [2] = {
        render = function()
            -- print("We are rendering in lua!")
        end,
    },
}
