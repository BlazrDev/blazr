-- Default Lua Script

local script = {}

local x = 5
local function animMoveRight()
    local animationComponent = Chammy:get_component(AnimationComponent)
    animationComponent.frame_offset = 3
end

local function animMoveLeft()
    local animationComponent = Chammy:get_component(AnimationComponent)
    animationComponent.frame_offset = 2
end

local function animIdleRight()
    local animationComponent = Chammy:get_component(AnimationComponent)
    animationComponent.frame_offset = 4
end

local function animIdleLeft()
    local animationComponent = Chammy:get_component(AnimationComponent)
    animationComponent.frame_offset = 5
end
local hit = false
local right = true
local opened = false
local heart = 2
local jumps = 0
local gameEnd = false
local function animHitRight()
    local animationComponent = Chammy:get_component(AnimationComponent)
    animationComponent.frame_offset = 0
    local boxCollider = Chammy:get_component(BoxColliderComponent)

    if hit == false then
        boxCollider.offset = vec2(boxCollider.offset.x + 5, boxCollider.offset.y)
        hit = true
        local mashaCollider = masha:get_component(BoxColliderComponent)
        local mashaTransform = masha:get_component(TransformComponent)
        local mashaSprite = masha:get_component(SpriteComponent)
        if BoxColliderSystem.is_colliding(masha, Chammy) then
            if heart == 0 then
                mashaTransform:set_position(2000,237)
            else 
                mashaSprite.color = vec4(255,0,0,0.5)
                heart = heart - 1
            end
        end

    end

end
local function animHitLeft()
    local animationComponent = Chammy:get_component(AnimationComponent)
    animationComponent.frame_offset = 1
    local boxCollider = Chammy:get_component(BoxColliderComponent)

    if hit == false then
        boxCollider.offset = vec2(boxCollider.offset.x - 5, boxCollider.offset.y)
        local mashaCollider = masha:get_component(BoxColliderComponent)
        local mashaTransform = masha:get_component(TransformComponent)
        local mashaSprite = masha:get_component(SpriteComponent)
        hit = true
        if BoxColliderSystem.is_colliding(masha, Chammy) then
            if heart == 0 then
                mashaTransform:set_position(2000,237)
            else 
                mashaSprite.color = vec4(255,0,0,0.5)
                heart = heart - 1
            end
        end

    end
end

local function returnCollider()
    if hit == true then
        local boxCollider = Chammy:get_component(BoxColliderComponent)
        
        if right == true then
            
            boxCollider.offset = vec2(boxCollider.offset.x - 5, boxCollider.offset.y)
        else
            boxCollider.offset = vec2(boxCollider.offset.x + 5, boxCollider.offset.y)
        end
        local mashaSprite = masha:get_component(SpriteComponent)
        mashaSprite.color = vec4(1,1,1,1)
        hit = false
    end
end


local function playFootStep()
    SoundPlayer.play_effect("zemlja.wav", 0)
end


local function printVel()
    local physicsComponent = Chammy:get_component(PhysicsComponent)
    local velocity = physicsComponent:get_linear_velocity()
    if math.abs(velocity.y) < 0.9 then
        jumps = 0
    end
end
footstepTimer = Timer()
footstepTimer:start()

t = Timer()
t:start()
local function update()
    local physicsComponent = Chammy:get_component(PhysicsComponent)
    local velocity = physicsComponent:get_linear_velocity()
    if InputSystem.key_repeating(KEY_D) then
        animMoveRight()
        physicsComponent:set_linear_velocity(vec2(x, velocity.y))
        right = true
        footstepTimer:execute_every(500, playFootStep)
    elseif InputSystem.key_repeating(KEY_A) then
        animMoveLeft()
        physicsComponent:set_linear_velocity(vec2(-x, velocity.y))
        right = false
        footstepTimer:execute_every(500, playFootStep)
    else
        if right == true then
            animIdleRight()
        else
            animIdleLeft()
        end
    end
    if InputSystem.key_repeating(KEY_SPACE) then
            SoundPlayer.play_effect("sword.wav", 0)

        local mashaCollider = masha:get_component(BoxColliderComponent)
        local mashaTransform = masha:get_component(TransformComponent)

        if right == true then
            animHitRight()
        else
            animHitLeft()
        end
    else
        returnCollider(right)
    end

    if InputSystem.key_pressed(KEY_W) then
        if jumps < 2 then
            physicsComponent:linear_impulse(vec2(0, 60))
           jumps = jumps + 1
        end
    end

    if InputSystem.key_pressed(KEY_R) or (InputSystem.key_pressed(KEY_ENTER) and gameEnd == true) then
        editor:SetSceneByName("mainScene")
        physicsComponent:set_transform(vec2(0, 100))
        opened = false
        local mashaTransform = masha:get_component(TransformComponent)
        mashaTransform:set_position(105,237)
        heart = 2
        jumps = 0
        gameEnd = false
    end
    t:execute_every(100, printVel)

end

local function chestOpenAnim()
    local animationComponent = chest:get_component(AnimationComponent)
    local animationCoinComponent = chestCoin:get_component(AnimationComponent)
    local transformCoinComponent = chestCoin:get_component(TransformComponent)
    if BoxColliderSystem.is_colliding(chest, Chammy) and opened == false then
        animationComponent.num_frames = 4
        animationComponent.frame_rate = 4
        if animationComponent.current_frame == 3 then
            animationComponent.frame_rate = 0
            opened = true
            transformCoinComponent:set_position(376, 167)
            SoundPlayer.play_effect("coin_flip.wav", 0)
        end
    else
        animationComponent.frame_rate = 0
    end

    if opened == true then
        animationCoinComponent.num_frames = 7
        animationCoinComponent.frame_rate = 4
        if animationCoinComponent.current_frame == 6 then
            transformCoinComponent:set_position(2000, 167)
        end
    end
end

SoundPlayer.play_music("Grasslands Theme.mp3", 1, 0)
SoundPlayer.set_music_volume("Grasslands Theme.mp3", 20)

local function isEnd()
    if BoxColliderSystem.is_colliding(house, Chammy) and heart == 0 and gameEnd == false then
        editor:SetSceneByName("endScene")
        gameEnd = true
    end
end

function script.on_update(entity)
    -- Your update logic here
    update()
    chestOpenAnim()
    isEnd()
end

function script.on_render(entity)
    -- Your render logic here
end

return script
