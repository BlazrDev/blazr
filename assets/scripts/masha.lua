-- Default Lua Script

local script = {}
local i = 0
function script.on_update(entity)
    -- Your update logic here
    -- print("hello world from lua")
    if i == 0 then
        print("NESTO SAM DODAO")
        i = 1
    end
end

function script.on_render(entity)
    -- Your render logic here
end

return script
