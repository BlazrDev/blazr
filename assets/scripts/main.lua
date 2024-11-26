-- Main Script
if run_script("assets/scripts/utilities.lua") then
	print("Utilities script has been executed")
end
if run_script("assets/tiled_maps/testmap.lua") then
	print("Testmap script has been executed")
end
Masha = {
	tag = "Masha",
	group = "Character2",
	components = {
		transform = {
			position = { x = 300, y = 300 },
			scale = { x = 0.3, y = 0.3 },
			rotation = 0,
		},
		sprite = {
			width = 472.0,
			height = 617.0,
			texture_path = "masha",
			start = { x = 0, y = 0 },
			layer = 0,
		},
		box_collider = {
			width = 472,
			height = 617,
			offset = { x = 0, y = 0 },
		},
	},
}
Player = {
	tag = "Player",
	group = "Character",
	components = {
		transform = {
			position = { x = 100, y = 100 },
			scale = { x = 5, y = 5 },
			rotation = 0,
		},
		sprite = {
			width = 32.0,
			height = 32.0,
			texture_path = "player",
			start = { x = 0, y = 0 },
			layer = 0,
		},
		box_collider = {
			width = 32,
			height = 32,
			offset = { x = 32, y = 0 },
		},
		animation = {
			num_frames = 10,
			frame_rate = 1,
			frame_offset = 10,
			b_vertical = false,
		},
	},
}

playerEntity = Entity("Player", "Character")
local transform = playerEntity:add_component(TransformComponent(300, 720, 5, 5, 0))
local sprite = playerEntity:add_component(SpriteComponent(32, 32, "player", 0, 0, 0))
local animation = playerEntity:add_component(AnimationComponent(10, 10, 10, false))
local collider = playerEntity:add_component(BoxColliderComponent(20, 32, vec2(20, 0)))
local attributes = PhysicsAtributes({
	type = RigidBodyType.Dynamic,
	density = 80,
	friction = 10,
	restitution = 0.3,
	gravityScale = 9.81,
	position = vec2(300, 1000),
	scale = vec2(5, 5),
	boxSize = vec2(20, 32),
	offset = vec2(20, 0),
	isSensor = false,
	isFixedRotation = true,
})
local physics = PhysicsComponent(attributes)

local physicsComponent = playerEntity:add_component(physics)

mashaEntity = Entity("Masha", "Character2")
local transform2 = mashaEntity:add_component(TransformComponent(-640, -360, 1, 1, 0))
local sprite2 = mashaEntity:add_component(SpriteComponent(640.0, 360.0, "texture", 0, 0, 0))
local collider2 = mashaEntity:add_component(BoxColliderComponent(640, 360, vec2(0, -50)))
local attributes2 = PhysicsAtributes({
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
local attributes3 = PhysicsAtributes({
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
	playerAnimation.frame_offset = 7
	playerAnimation.current_frame = 0
	playerAnimation.num_frames = 11
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
	else
		stoji()
	end
	if InputSystem.key_repeating(KEY_D) then
		physicsComponent:set_linear_velocity(vec2(25, velocity.y))
		trci()
	else
		stoji()
	end
	if InputSystem.key_repeating(KEY_SPACE) then
		physicsComponent:set_linear_velocity(vec2(velocity.x, 0))
		physicsComponent:set_linear_impulse(vec2(0, -1000000))
		skace()
	else
		stoji()
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
