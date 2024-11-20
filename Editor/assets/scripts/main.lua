-- Main Script

playerEntity = Entity("Player", "Character")
--
<<<<<<< HEAD
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
            if InputSystem.key_pressed(KEY_A) then
                transform2.position.x = transform2.position.x - 1
=======
local transform = playerEntity:add_component(TransformComponent(100, 100, 5, 5, 0))
local sprite = playerEntity:add_component(SpriteComponent(32.0, 32.0, "player", 0, 0, 0))
local animation = playerEntity:add_component(AnimationComponent(10, 10, 3, false))

mashaEntity = Entity("Masha", "Character")
local transform2 = mashaEntity:add_component(TransformComponent(300, 300, 0.3, 0.3, 0))
local sprite2 = mashaEntity:add_component(SpriteComponent(472.0, 617.0, "masha", 0, 0, 0))

sprite:generate_object()
sprite2:generate_object()

local function stoji()
    animation.frame_offset = 7
    animation.current_frame = 0
    animation.num_frames = 11
end

local function skace()
    animation.frame_offset = 2
    animation.current_frame = 0
    animation.num_frames = 6
end

local function trci()
    animation.frame_offset = 6
    animation.current_frame = 0
    animation.num_frames = 8
end
main = {
    [1] = {
        update = function()
            if InputSystem.key_repeating(KEY_A) then
                transform.position.x = transform.position.x - 5
            elseif InputSystem.key_repeating(KEY_D) then
                transform.position.x = transform.position.x + 5
                trci()
            elseif InputSystem.key_repeating(KEY_S) then
                transform.position.y = transform.position.y + 5
            elseif InputSystem.key_repeating(KEY_W) then
                transform.position.y = transform.position.y - 5
            else
                stoji()
            end
            if InputSystem.key_repeating(KEY_SPACE) then
                skace()
>>>>>>> BLAZRDEV-48-editor
            end
        end,
    },
    [2] = {
        render = function()
            -- stoji()
            -- print("We are rendering in lua!")
        end,
    },
}