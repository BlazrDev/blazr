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
-- soundPlayer:play_music("masa", 1, 0)
-- soundPlayer:set_channel_volume(1, 10)
-- print(soundPlayer:get_channel_volume(1))
-- soundPlayer:play_effect("masa", 0, 1)
-- soundPlayer:mute_all_effects()

local x = 0
local max_x = 800

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
                soundPlayer:toggle_music_mute()
                soundPlayer:play_effect("jump", 0, 1)
                -- print(soundPlayer:get_channel_volume(1))
                -- soundPlayer:set_channel_volume(1, soundPlayer:get_channel_volume(1) + 10)
                -- soundPlayer:set_music_volume(soundPlayer:get_music_volume() + 10)
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
