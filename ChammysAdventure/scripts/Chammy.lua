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

local right = true
local opened = false

local function update()
    local physicsComponent = Chammy:get_component(PhysicsComponent)
    local velocity = physicsComponent:get_linear_velocity()
    if InputSystem.key_repeating(KEY_D) then
        animMoveRight()
        physicsComponent:set_linear_velocity(vec2(x, velocity.y))
        right = true
    elseif InputSystem.key_repeating(KEY_A) then
        animMoveLeft()
        physicsComponent:set_linear_velocity(vec2(-x, velocity.y))
        right = false
    else

        if right == true then
            animIdleRight()
        else 
            animIdleLeft()
        end
    end



    if InputSystem.key_pressed(KEY_SPACE) then
        physicsComponent:linear_impulse(vec2(0, 60))
    end

    if InputSystem.key_pressed(KEY_R) then
        physicsComponent:set_transform(vec2(0, 100))
        opened = false
    end
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
                transformCoinComponent:set_position(376,167)
            end
    else
        animationComponent.frame_rate = 0
    end

    if opened == true then
        animationCoinComponent.num_frames = 7
        animationCoinComponent.frame_rate = 4
        if animationCoinComponent.current_frame == 6 then
            transformCoinComponent:set_position(2000,167)
        end
    end

end



function script.on_update(entity)
    -- Your update logic here
    update()
    chestOpenAnim()

end

function script.on_render(entity)
    -- Your render logic here
end

return script
