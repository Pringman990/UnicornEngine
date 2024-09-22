----------------------------------------------------------------------------
-- the dirs table is a listing of absolute paths, since we generate projects
-- and files it makes a lot of sense to make them absolute to avoid problems
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

dirs = {}
dirs["root"] 				= os.realpath("../../")

--Roots
dirs["Binaries"]			= os.realpath(dirs.root .. "Binaries/")
dirs["Intermediate"]		= os.realpath(dirs.root .. "Intermediate/")
dirs["Source"] 				= os.realpath(dirs.root .. "Source/")

--Binaries
dirs["Binaries_Shaders"]	= os.realpath(dirs.Binaries .. "Shaders/")
dirs["Binaries_Win64"]		= os.realpath(dirs.Binaries .. "Win64/")
dirs["Binaries_ThirdParty"]	= os.realpath(dirs.Binaries .. "ThirdParty/")

--Intermediates
dirs["Inter_Temp"]			= os.realpath(dirs.Intermediate .. "Temp/")
dirs["Inter_ProjectFiles"]	= os.realpath(dirs.Intermediate .. "ProjectFiles/")

--Default Directories
defaultTargetName = "%{prj.name}_%{cfg.buildcfg}"
defaultTargetDir = dirs.Inter_Temp .. "%{cfg.buildcfg}"
defaultObjDir = "%{dirs.Inter_Temp}/%{prj.name}/%{cfg.buildcfg}"
defaultLocationDir = dirs.Inter_ProjectFiles

--Functions
function normalizePath(path)
    -- Normalize slashes (optional depending on your environment)
    path = path:gsub("\\", "/")

    -- Remove trailing slashes if present
    path = path:gsub("/+$", "")

    -- Find the last slash in the path
    local lastSlash = path:match(".*()/")

    if lastSlash then
        -- Return everything before the last slash
        return path:sub(1, lastSlash - 1)
    else
        -- If no slash is found, return an empty string (or handle it differently if needed)
        return ""
    end
end

projectInheritDirs = {}

function inheritAndIncludeDirsFromProject(projectname)
    -- Check if the project has inheritable directories defined
   if projectInheritDirs[projectname] then
      -- Add the inherited directories to the include directories
      return projectInheritDirs[projectname]
   else
     print("Project " .. projectname .. " has no inheritable directories.")
   end
end

function flattenTable(t)
    local flatTable = {}
    for _, v in ipairs(t) do
        if type(v) == "table" then
            for _, nestedV in ipairs(v) do
                table.insert(flatTable, nestedV)
            end
        else
            table.insert(flatTable, v)
        end
    end
    return flatTable
end