-- Main Script
--
gEntity = Entity("E1", "G1")
local transform2 = gEntity:add_component(TransformComponent(100, 300, 1, 1, 0))
gEntity:add_component(SpriteComponent(50, 50, "chammy", 0, 0, 0))

gEntity = Entity("E1", "G1")
local transform = gEntity:add_component(TransformComponent(100, 300, 1, 1, 0))
local sprite = gEntity:add_component(SpriteComponent(200, 200, "masha", 0, 0, 0))

-- local transoform = gEntity:add_component(TransformComponent(100, 100, 0, 0, 0))
-- print(transoform.scale)
local radius = 150 -- Poluprečnik kruga
local centerX = 400 -- X koordinata centra kruga
local centerY = 300 -- Y koordinata centra kruga
local steps = 100
local i = 0
main = {
    [1] = {
        update = function()
            if i == steps then
                i = 0
            end
            local angle = (i / steps) * (2 * math.pi) -- Ugao u radijanima za svaku iteraciju
            local x = centerX + radius * math.cos(angle)
            local y = centerY + radius * math.sin(angle)
            i = i + 1
            transform2:set_position(x + 130, y + 45)
            transform:set_position(x, y)
        end,
    },
    [2] = {
        render = function()
            -- print("We are rendering in lua!")
        end,
    },
}
