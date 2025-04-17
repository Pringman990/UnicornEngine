----------------------------------------------------------------------------
-- the dirs table is a listing of absolute paths, since we generate projects
-- and files it makes a lot of sense to make them absolute to avoid problems
-- outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

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
dirs["Inter_Lib"]	        = os.realpath(dirs.Intermediate .. "Lib/")

UCE_TARGET_NAME = "%{prj.name}_%{cfg.buildcfg}"
UCE_TARGET_DIR = dirs.Inter_Lib .. "%{cfg.buildcfg}"

UCE_OBJ_DIR = dirs.Inter_Temp .. "%{prj.name}/%{cfg.buildcfg}"
UCE_VCXPROJ_DIR = dirs.Inter_ProjectFiles

UCE_DLL_DIR = dirs.Binaries_ThirdParty
UCE_EXECUTABLE_DIR = dirs.Binaries_Win64

UCE_BINARIES_DIR = dirs.Binaries
UCE_SOURCE_DIR = dirs.Source

UCE_COMPILED_SHADERS_DIR = dirs.Binaries_Shaders

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

-- Table to store dependencies for each project
local linksDependencyGraph = {}

-- Recursive function to resolve all transitive dependencies
function resolveLinksDependencies(projectName, resolved, unresolved)
    if unresolved[projectName] then
        error("Circular dependency detected involving " .. projectName)
    end

    if not resolved[projectName] then
        unresolved[projectName] = true
        local dependencies = linksDependencyGraph[projectName] or {}
        for _, dependency in ipairs(dependencies) do
            resolveLinksDependencies(dependency, resolved, unresolved)
        end
        unresolved[projectName] = nil
        resolved[projectName] = true
    end
end

-- Function to link and register dependencies in one step
function linkDependencies(dependent, dependencies)
    -- Register dependencies
    linksDependencyGraph[dependent] = linksDependencyGraph[dependent] or {}
    for _, dependency in ipairs(dependencies) do
        table.insert(linksDependencyGraph[dependent], dependency)
    end

    -- Resolve dependencies recursively and propagate links
    local resolved = {}
    resolveLinksDependencies(dependent, resolved, {})

    project(dependent)

    for dependency, _ in pairs(resolved) do
        if dependency ~= dependent then
            links { dependency }
           -- print("--Link Dependecy: " .. dependency)
        end
    end
end

local includesDependencyGraph = {}

-- Recursive function to resolve all transitive dependencies
function resolveIncludesDependencies(projectName, resolved, unresolved)
    if unresolved[projectName] then
        error("Circular dependency detected involving " .. projectName)
    end

    if not resolved[projectName] then
        unresolved[projectName] = true
        local dependencies = includesDependencyGraph[projectName] or {}
        for _, dependency in ipairs(dependencies) do   
            resolveIncludesDependencies(dependency, resolved, unresolved)
        end

        unresolved[projectName] = nil
        resolved[projectName] = true
    end
end

function includeDependencies(dependent, dependencies)

    includesDependencyGraph[dependent] = includesDependencyGraph[dependent] or {}

    for _, dependency in ipairs(dependencies) do
        if dependency ~= dependent then
            if not includesDependencyGraph[dependency] then
                table.insert(includesDependencyGraph[dependent], dependency)
            else
                for _, path in pairs(includesDependencyGraph[dependency]) do
                    table.insert(includesDependencyGraph[dependent], path)
                end   
            end

        end
    end

    -- Resolve dependencies recursively
    local resolved = {}
    resolveIncludesDependencies(dependent, resolved, {})

    -- Apply include paths to the dependent project
    project(dependent)
    for resolvedDep, _ in pairs(resolved) do
        if resolvedDep ~= dependent then
            includedirs { resolvedDep }
            print("--Include Path: " .. resolvedDep)
        end
    end
end

