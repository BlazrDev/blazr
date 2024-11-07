-- Main Script
--
gEntity = Entity("E1", "G1")

local transoform = gEntity:add_component(TransformComponent(100, 100, 0, 0, 0))
-- print(transoform.scale)

main = {
    [1] = {
        update = function()
            -- print("We are updating in lua!")
        end,
    },
    [2] = {
        render = function()
            -- print("We are rendering in lua!")
        end,
    },
}
