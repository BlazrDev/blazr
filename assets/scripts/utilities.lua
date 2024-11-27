function LoadEntity(def)
	assert(def, "Def does not exist")

	local tag = ""
	if def.tag then
		tag = def.tag
	end

	local group = ""
	if def.group then
		group = def.group
	end

	local ent = Entity(tag, group)

	if def.components.transform then
		print("Entity has a transform component")
		local transformComponent = ent:add_component(
			TransformComponent(
				def.components.transform.position.x,
				def.components.transform.position.y,
				def.components.transform.scale.x,
				def.components.transform.scale.y,
				def.components.transform.rotation
			)
		)
	end

	if def.components.sprite then
		print("Entity has a sprite component")
		local spriteComponent = ent:add_component(
			SpriteComponent(
				def.components.sprite.width,
				def.components.sprite.height,
				def.components.sprite.texture_path,
				def.components.sprite.start.x,
				def.components.sprite.start.y,
				def.components.sprite.layer
			)
		)
		spriteComponent:generate_object()
	end

	if def.components.box_collider then
		print("Entity has a box_collider component")
		ent:add_component(
			BoxColliderComponent(
				def.components.box_collider.width,
				def.components.box_collider.height,
				vec2(def.components.box_collider.offset.x, def.components.box_collider.offset.y)
			)
		)
	end

	if def.components.animation then
		print("Entity has a animation component")
		ent:add_component(
			AnimationComponent(
				def.components.animation.num_frames,
				def.components.animation.frame_rate,
				def.components.animation.frame_offset,
				def.components.animation.b_vertical
			)
		)
	end
	return ent
end

function LoadAssets(def)
	assert(def, "Def does not exist!")

	if def.music then
		for k, v in pairs(def.music) do
			if not AssetManager.load_music(v.name, v.path, v.desc) then
				print("Failed to load music " .. v.name)
			else
				print("Music loaded" .. v.name)
			end
		end
	end
	if def.sound_effects then
		for k, v in pairs(def.sound_effects) do
			if not AssetManager.load_effect(v.name, v.path, v.desc) then
				print("Failed to load sound effect " .. v.name)
			else
				print("Sound effect loaded " .. v.name)
			end
		end
	end
	if def.textures then
		for k, v in pairs(def.textures) do
			if not AssetManager.load_texture(v.name, v.path, v.pixelArt) then
				print("Failed to load texture " .. v.name)
			else
				print("Texture loaded " .. v.name)
			end
		end
	end
end

Tileset = {}

Tileset.__index = Tileset
function Tileset:Create(params)
	local this = {
		name = params.name,
		columns = params.columns,
		width = params.width,
		height = params.height,
		tilewidth = params.tilewidth,
		tileheight = params.tileheight,
		firstgid = params.firstgid,
	}
	this.rows = params.height / params.tileheight
	this.lastgid = math.floor(((this.rows * this.columns) + this.firstgid - 1))
	print("debilu")
	print(this.rows, this.lastgid)
	setmetatable(this, self)
	return this
end

function Tileset:TileIdExists(id)
	print("nnjesto")
	return id >= self.firstgid and id <= self.lastgid
end

--function that gets the coords of a single tile from a spritesheete
function Tileset:GetTileCoords(id)
	assert(self:TileIdExists(id), "Tile with id: " .. id .. "does not exist in the tileset: " .. self.name)
	local actualId = id - self.firstgid
	local startY = math.floor(1440 / 16) - math.floor(actualId / self.columns) - 1
	local startX = math.floor(actualId % self.columns)
	print("start x: " .. startX .. " start y: " .. startY)
	return startX, startY
end

function LoadMap(mapDef)
	assert(mapDef, "Map does not exist")

	local tilesets = {}
	for k, v in pairs(mapDef.tilesets) do
		local newTileset = Tileset:Create({
			name = v.name,
			columns = v.columns,
			width = v.imagewidth,
			height = v.imageheight,
			tilewidth = v.tilewidth,
			tileheight = v.tileheight,
			firstgid = v.firstgid,
		})
		print(newTileset.rows)
		table.insert(tilesets, newTileset)
	end

	--looping through the layers and creteing separate tilesets
	for k, v in pairs(mapDef.layers) do
		local rows = v.height - 1
		local columns = v.width
		local layer = k - 1
		--data is a 1D array
		for row = 0, rows do
			for col = 1, columns do
				local id = v.data[row * columns + col]

				--if id is zero that means there is no tile there at that location, so we need to skip it
				if id == 0 then
					goto continue
				end
				local tileset = GetTilesets(tilesets, id)
				assert(tileset, "Tileset does not exist with id: " .. id)

				local startX, startY = tileset:GetTileCoords(id)
				--for now scale is 2
				local scale = 2
				local tile = Entity("", "tiles")
				tile:add_component(
					TransformComponent(
						((col - 1) * tileset.tilewidth * scale),
						(row * tileset.tileheight * scale),
						scale,
						scale,
						0
					)
				)
				local sprite = tile:add_component(
					SpriteComponent(tileset.tilewidth, tileset.tileheight, tileset.name, startX, startY, layer)
				)
				sprite:generate_object()
				if tileset.name == "collider" then
					tile:add_component(BoxColliderComponent(tileset.tilewidth, tileset.tileheight, vec2(0, 0)))
					tile:add_component(PhysicsComponent(PhysicsAtributes({position = vec2(
						((col - 1) * tileset.tilewidth * scale),
						(row * tileset.tileheight * scale)
					),
					boxSize = vec2(tileset.tilewidth, tileset.tileheight)})))
				end

				::continue::
			end
		end
	end
end

function GetTilesets(tilesets, id)
	for k, v in pairs(tilesets) do
		if v:TileIdExists(id) then
			return v
		end
	end
	return nil
end
