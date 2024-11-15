-- Main Script
--
-- local transform = TransformComponent(100, 100, 1, 1, 10)
-- gEntity:add_component(transform)
-- -- -- gEntity:add_component(SpriteComponent(200, 200, "chammy", 0, 0, 0))
-- -

gEntity2 = Entity("E2", "G1")
local transform2 = gEntity2:add_component(TransformComponent(vec2(100, 300), vec2(1, 1), 10))
local sprite2 = gEntity2:add_component(SpriteComponent(200, 200, "masha", 0, 0, 0))
local collider2 = gEntity2:add_component(BoxColliderComponent(200, 200, vec2(0, 0)))

collider2.colliding = true

playerEntity = Entity("Player", "G2")
local transform = playerEntity:add_component(TransformComponent(500, 450, 1, 1, 0))
local sprite = playerEntity:add_component(SpriteComponent(32.0, 32.0, "chammy", 0, 0, 0))
local collider = playerEntity:add_component(BoxColliderComponent(200, 200, vec2(0, 0)))
collider.colliding = true

local x = 0
local max_x = 800
main = {
    [1] = {
        update = function()
            if x < max_x then
                transform2.position.x = x
                transform2.position.y = 300.0
                -- transform2:set_position(x, 300.0)
                x = x + 20
            end

            if x >= max_x then
                x = 0
            end

            if BoxColliderSystem.is_colliding(gEntity2, playerEntity) then
                print("Colliding")
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
