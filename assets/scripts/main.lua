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

-- playerEntity = Entity("Player", "Character")
--
Assets = {
	music = {
		{ name = "masa", path = "assets/sounds/masa.mp3", desc = "Masa background music" },
		{ name = "metak", path = "assets/sounds/METAK.mp3", desc = "Metak background music" },
	},
	sound_effects = {
		{ name = "boing", path = "assets/sounds/boing.wav", desc = "boingggggggggg" },
	},
	textures = {
		{ name = "masha", path = "assets/masha.png", pixelArt = false },
		{ name = "chammy", path = "assets/chammy.png", pixelArt = false },
		{ name = "player", path = "assets/sprite_sheet.png", pixelArt = false },
		{ name = "map", path = "assets/map_assets/map.png", pixelArt = true },
		{ name = "collider", path = "assets/map_assets/collider.png", pixelArt = true },
	},
}

local tileset = CreateMap()
LoadAssets(Assets)
LoadMap(tileset)
-- local transform = playerEntity:add_component(TransformComponent(100, 100, 5, 5, 0))
-- local sprite = playerEntity:add_component(SpriteComponent(32.0, 32.0, "player", 0, 0, 0))
-- local animation = playerEntity:add_component(AnimationComponent(10, 1, 10, false))
-- local collider = playerEntity:add_component(BoxColliderComponent(32, 32, vec2(32, 0)))
--
-- mashaEntity = Entity("Masha", "Character2")
-- local transform2 = mashaEntity:add_component(TransformComponent(300, 200, 0.3, 0.3, 0))
-- local sprite2 = mashaEntity:add_component(SpriteComponent(472.0, 617.0, "masha", 0, 0, 0))
-- local collider2 = mashaEntity:add_component(BoxColliderComponent(472, 617, vec2(0, 0)))
local mashaEntity = LoadEntity(Masha)
local playerEntity = LoadEntity(Player)
local playerCollider = playerEntity:get_component(BoxColliderComponent)
local mashaCollider = mashaEntity:get_component(BoxColliderComponent)
local playerAnimation = playerEntity:get_component(AnimationComponent)
local playerTransform = playerEntity:get_component(TransformComponent)
-- explosionntity = Entity("Explosion", "e")
-- local transform3 = mashaEntity:add_component(TransformComponent(300, 200, 0.3, 0.3, 0))
-- local sprite3 = mashaEntity:add_component(SpriteComponent(472.0, 617.0, "masha", 0, 0, 0))
-- local animation3 = playerEntity:add_component(AnimationComponent(10, 10, 10, false))

-- sprite:generate_object()
-- sprite2:generate_object()
local lifes = 3
playerCollider.colliding = true
mashaCollider.colliding = true
-- collider.colliding = true
-- collider2.colliding = true

local function stoji()
	playerAnimation.frame_offset = 7
	playerAnimation.current_frame = 0
	playerAnimation.num_frames = 11
end

local function udara()
	playerAnimation.frame_offset = 4

	playerAnimation.current_frame = 2
	playerAnimation.num_frames = 10
end

local function trci()
	playerAnimation.frame_offset = 6
	playerAnimation.current_frame = 0
	playerAnimation.num_frames = 8
	-- sprite2.color.x = 1.0
	-- sprite2.color.y = 1.0
	-- sprite2.color.z = 1.0
end
-- local am = AssetManager:get_instance()
-- AssetManager.load_music("masa", "assets/sounds/masa.mp3", "dess")
-- AssetManager.load_effect("boing", "assets/sounds/boing.wav", "binggg")
-- SoundPlayer.play_music("masa", 0, 0)
-- run_script("assets/scripts/test.lua")

main = {
	[1] = {
		update = function()
			if InputSystem.key_repeating(KEY_A) then
				playerTransform.position.x = playerTransform.position.x - 5
			elseif InputSystem.key_repeating(KEY_D) then
				playerTransform.position.x = playerTransform.position.x + 5
				trci()
			elseif InputSystem.key_repeating(KEY_S) then
				playerTransform.position.y = playerTransform.position.y + 5
			elseif InputSystem.key_repeating(KEY_W) then
				playerTransform.position.y = playerTransform.position.y - 5
			else
				stoji()
			end
			if InputSystem.key_repeating(KEY_SPACE) then
				udara()
				-- SoundPlayer.play_effect("boing", 0, 1)
			end

			if BoxColliderSystem.is_colliding(playerEntity, mashaEntity) then
				print("Colliding")
				-- sprite2.color.x = 1.0
				-- sprite2.color.y = 0.0
				-- sprite2.color.z = 0.0
				playerTransform.position.x = playerTransform.position.x - 5
			end
			playerCollider.colliding = false
			mashaCollider.colliding = false
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
