--Core Root Directories
print("Including Build/")

project "BUILD"
	language "C++"
	cppdialect "C++20"
	kind "StaticLib"

	targetname(UCE_TARGET_NAME)
    targetdir (UCE_TARGET_DIR)

    objdir(UCE_OBJ_DIR)
    location (UCE_VCXPROJ_DIR)

	files {
		"build.cpp"
	}

	if not os.isfile("build.cpp") then
        io.writefile("build.cpp", "")
    end

	-- Common post-build command to copy general DLLs
   --postbuildcommands {
   --   "echo Copying general DLLs from " .. dirs.DLL .. " to " .. dirs.bin,
   --   "xcopy /Q /Y /I \"" .. dirs.DLL .. "\\*.dll\" \"" .. dirs.bin .. "\\\""
   --}

   filter {"configurations:MemoryDebug"}
      -- Additional post-build command to copy debug-specific DLLs
      postbuildcommands {
         "echo Copying debug DLLs from " .. UCE_DLL_DIR .. "/debug to " .. UCE_EXECUTABLE_DIR,
         "xcopy /Q /Y /I \"" .. UCE_DLL_DIR .. "\\debug\\*.dll\" \"" .. UCE_EXECUTABLE_DIR .. "\\\"",
         "xcopy /Q /Y /I \"" .. UCE_DLL_DIR .. "\\debug\\*.pdb\" \"" .. UCE_EXECUTABLE_DIR .. "\\\""
      }

    filter ("configurations:MemoryDebug Editor")
      postbuildcommands {
         "echo Copying debug DLLs from " .. UCE_DLL_DIR .. "/debug to " .. UCE_EXECUTABLE_DIR,
         "xcopy /Q /Y /I \"" .. UCE_DLL_DIR .. "\\debug\\*.dll\" \"" .. UCE_EXECUTABLE_DIR .. "\\\"",
         "xcopy /Q /Y /I \"" .. UCE_DLL_DIR .. "\\debug\\*.pdb\" \"" .. UCE_EXECUTABLE_DIR .. "\\\""
      }

    filter ("configurations:Debug")
      postbuildcommands {
         "echo Copying debug DLLs from " .. UCE_DLL_DIR .. "/debug to " .. UCE_EXECUTABLE_DIR,
         "xcopy /Q /Y /I \"" .. UCE_DLL_DIR .. "\\debug\\*.dll\" \"" .. UCE_EXECUTABLE_DIR .. "\\\"",
         "xcopy /Q /Y /I \"" .. UCE_DLL_DIR .. "\\debug\\*.pdb\" \"" .. UCE_EXECUTABLE_DIR .. "\\\""
      }
      
    filter ("configurations:Debug Editor")
      postbuildcommands {
         "echo Copying debug DLLs from " .. UCE_DLL_DIR .. "/debug to " .. UCE_EXECUTABLE_DIR,
         "xcopy /Q /Y /I \"" .. UCE_DLL_DIR .. "\\debug\\*.dll\" \"" .. UCE_EXECUTABLE_DIR .. "\\\"",
         "xcopy /Q /Y /I \"" .. UCE_DLL_DIR .. "\\debug\\*.pdb\" \"" .. UCE_EXECUTABLE_DIR .. "\\\""
      }

   filter "configurations:Retail"
      -- Additional post-build command to copy release-specific DLLs
      postbuildcommands {
         "echo Copying release DLLs from " .. UCE_DLL_DIR .. "/release to " .. UCE_EXECUTABLE_DIR,
         "xcopy /Q /Y /I \"" .. UCE_DLL_DIR .. "\\release\\*.dll\" \"" .. UCE_EXECUTABLE_DIR .. "\\\""
      }
      print("Output directory: " .. UCE_EXECUTABLE_DIR)

filter {}