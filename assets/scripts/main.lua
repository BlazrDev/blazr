-- Main Script

playerEntity = Entity("Plaer", "Character")
--
local transform = playerEntity:add_component(TransformComponent(100, 100, 10, 10, 0))
local sprite = playerEntity:add_component(SpriteComponent(32.0, 32.0, "player", 0, 0, 0))
local transform = playerEntity:add_component(AnimationComponent(4, 10, 0, false))
sprite:generate_object()

main = {
    [1] = {
        update = function() end,
    },
    [2] = {
        render = function()
            -- print("We are rendering in lua!")
        end,
    },
}
