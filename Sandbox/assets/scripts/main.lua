-- Main Script

playerEntity = Entity("Player", "Character")
--
local transform = playerEntity:add_component(TransformComponent(100, 100, 10, 10, 0))
local sprite = playerEntity:add_component(SpriteComponent(32.0, 32.0, "player", 0, 0, 0))
local transform = playerEntity:add_component(AnimationComponent(10, 10, 3, false))

mashaEntity = Entity("Masha", "Character")
local transform2 = mashaEntity:add_component(TransformComponent(300, 300, 10, 10, 0))
local sprite2 = mashaEntity:add_component(SpriteComponent(472.0, 617.0, "masha", 0, 0, 0))

sprite:generate_object()
sprite2:generate_object()

main = {
    [1] = {
        update = function()
<<<<<<< HEAD
            if InputSystem.key_pressed(KEY_A) then
                transform2.position.x = transform2.position.x - 1
            end
=======
>>>>>>> origin/BLAZRDEV-50-Scene
        end,
    },
    [2] = {
        render = function()
<<<<<<< HEAD
            -- print("We are rendering in lua!")
=======
>>>>>>> origin/BLAZRDEV-50-Scene
        end,
    },
}
