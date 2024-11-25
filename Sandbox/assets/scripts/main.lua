-- Main Script
--
-- local transform = TransformComponent(100, 100, 1, 1, 10)
-- gEntity:add_component(transform)
-- -- -- gEntity:add_component(SpriteComponent(200, 200, "chammy", 0, 0, 0))
-- --
gEntity2 = Entity("E2", "G1")
local transform2 = gEntity2:add_component(TransformComponent(vec2(100, 300), vec2(1, 1), 10))
local sprite2 = gEntity2:add_component(SpriteComponent(200, 200, "masha", 0, 0, 0))

local s2 = gEntity2:get_component(SpriteComponent)
print(s2.width)
local soundPlayer = SoundPlayer.get_instance()

local x = 0
local max_x = 800

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
            -- print(soundPlayer:is_playing(1))
            if x < max_x then
                transform2.position.x = x
                transform2.position.y = 300.0
                -- transform2:set_position(x, 300.0)
                x = x + 20
            end

            if x >= max_x then
                x = 0
                soundPlayer:play_effect("boing", 0, 1)
            end

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
