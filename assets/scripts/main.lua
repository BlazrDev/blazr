-- Main Script

playerEntity = Entity("Player", "Character")
--
local transform = playerEntity:add_component(TransformComponent(100, 100, 5, 5, 0))
local sprite = playerEntity:add_component(SpriteComponent(32.0, 32.0, "player", 0, 0, 0))
local animation = playerEntity:add_component(AnimationComponent(10, 1, 10, false))
local collider = playerEntity:add_component(BoxColliderComponent(32, 32, vec2(32, 0)))

mashaEntity = Entity("Masha", "Character2")
local transform2 = mashaEntity:add_component(TransformComponent(300, 200, 0.3, 0.3, 0))
local sprite2 = mashaEntity:add_component(SpriteComponent(472.0, 617.0, "masha", 0, 0, 0))
local collider2 = mashaEntity:add_component(BoxColliderComponent(472, 617, vec2(0, 0)))

-- explosionntity = Entity("Explosion", "e")
-- local transform3 = mashaEntity:add_component(TransformComponent(300, 200, 0.3, 0.3, 0))
-- local sprite3 = mashaEntity:add_component(SpriteComponent(472.0, 617.0, "masha", 0, 0, 0))
-- local animation3 = playerEntity:add_component(AnimationComponent(10, 10, 10, false))

sprite:generate_object()
sprite2:generate_object()

local lifes = 3
collider.colliding = true
collider2.colliding = true

local function stoji()
	animation.frame_offset = 7
	animation.current_frame = 0
	animation.num_frames = 11
end

local function udara()
	animation.frame_offset = 4

	animation.current_frame = 2
	animation.num_frames = 10
end

local function trci()
	animation.frame_offset = 6
	animation.current_frame = 0
	animation.num_frames = 8
	sprite2.color.x = 1.0
	sprite2.color.y = 1.0
	sprite2.color.z = 1.0
end
-- local am = AssetManager:get_instance()
AssetManager.load_music("masa", "assets/sounds/masa.mp3", "dess")
-- AssetManager.load_effect("boing", "assets/sounds/boing.wav", "binggg")
SoundPlayer.play_music("masa", 0, 0)
-- run_script("assets/scripts/test.lua")

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
				udara()
				SoundPlayer.play_effect("boing", 0, 1)
			end

			if BoxColliderSystem.is_colliding(playerEntity, mashaEntity) then
				print("Colliding")
				sprite2.color.x = 1.0
				sprite2.color.y = 0.0
				sprite2.color.z = 0.0
				transform.position.x = transform.position.x - 5
			end
			-- collider.colliding = false
			-- collider2.colliding = false
		end,
	},
	[2] = {
		render = function()
			-- stoji()
			-- stoji()
			-- print("We are rendering in lua!")
		end,
	},
}
