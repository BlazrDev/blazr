-- Main Script

playerEntity = Entity("Player", "Character")
-- --
local transform = playerEntity:add_component(TransformComponent(300, 720, 5, 5, 0))
local sprite = playerEntity:add_component(SpriteComponent(32, 32, "player", 0, 0, 0))
local animation = playerEntity:add_component(AnimationComponent(10, 10, 10, false))
local collider = playerEntity:add_component(BoxColliderComponent(20, 32, vec2(20, 0)))
local attributes = PhysicsAttributes({
    type = RigidBodyType.Dynamic,
    density = 80,
    friction = 10,
    restitution = 0.3,
    gravityScale = 9.81,
    position = transform.position,
    scale = transform.scale,
    boxSize = vec2(collider.width, collider.height),
    offset = collider.offset,
    isSensor = false,
    isFixedRotation = true,
})
local physics = PhysicsComponent(attributes)

local physicsComponent = playerEntity:add_component(physics)

mashaEntity = Entity("Masha", "Character2")
local transform2 = mashaEntity:add_component(TransformComponent(-640, -360, 1, 1, 0))
local sprite2 = mashaEntity:add_component(SpriteComponent(640.0, 360.0, "texture", 0, 0, 0))
local collider2 = mashaEntity:add_component(BoxColliderComponent(640, 360, vec2(0, -50)))
local attributes2 = PhysicsAttributes({
    type = RigidBodyType.Static,
    density = 1000,
    friction = 0.5,
    restitution = 0,
    gravityScale = 0,
    position = vec2(-640, -360),
    scale = vec2(1, 1),
    boxSize = vec2(640, 360),
    offset = vec2(0, -50),
    isSensor = false,
    isFixedRotation = false,
})
local physics2 = PhysicsComponent(attributes2)

local physicsComponent2 = mashaEntity:add_component(physics2)

texture = Entity("Texture", "Character3")
local transform3 = texture:add_component(TransformComponent(200, -360, 1, 1, 0))
local sprite3 = texture:add_component(SpriteComponent(640.0, 360.0, "texture", 0, 0, 0))
local collider3 = texture:add_component(BoxColliderComponent(640, 360, vec2(0, -50)))
local attributes3 = PhysicsAttributes({
    type = RigidBodyType.Static,
    density = 1000,
    friction = 0.5,
    restitution = 0,
    gravityScale = 0,
    position = vec2(200, -320),
    scale = vec2(1, 1),
    boxSize = vec2(640, 360),
    offset = vec2(0, -50),
    isSensor = false,
    isFixedRotation = false,
})
local physics3 = PhysicsComponent(attributes3)

local physicsComponent3 = texture:add_component(physics3)

sprite:generate_object()
sprite2:generate_object()
sprite3:generate_object()
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
    -- sprite2.color.x = 1.0
    -- sprite2.color.y = 1.0
    -- sprite2.color.z = 1.0
end

collider.colliding = true
collider2.colliding = true

local x = -100

local function update()
    local velocity = physicsComponent:get_linear_velocity()
    if InputSystem.key_repeating(KEY_A) then
        physicsComponent:set_linear_velocity(vec2(-25, velocity.y))
        trci()
    end
    if InputSystem.key_repeating(KEY_D) then
        physicsComponent:set_linear_velocity(vec2(25, velocity.y))
        trci()
    end
    if InputSystem.key_repeating(KEY_SPACE) then
        physicsComponent:set_linear_velocity(vec2(velocity.x, 0))
        physicsComponent:set_linear_impulse(vec2(0, -1000000))
        skace()
    end
end

main = {
    [1] = {
        update = function()
            update()
        end,
    },
    [2] = {
        render = function()
            -- print("We are rendering in lua!")
        end,
    },
}
