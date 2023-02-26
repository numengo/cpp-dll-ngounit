--****************************************************************
--*    Author:      Cedric ROMAN (roman@numengo.com
--*    Date:        13/02/2017
--****************************************************************

function convertPath(oldpath)
    if (os.istarget("windows")) then
        newpath = path.translate(oldpath, "\\")
        if (not string.startswith(_ACTION,"vs")) then
             --newpath = path.translate(newpath, "\\\\")
             newpath = path.translate(newpath, "/")
        end
        return newpath
    end
    return path.translate(oldpath, "/")
end

function checkAndGetEnv(var)
    value = os.getenv(var)
    if (value == nil) then
        print("Error !! "..var.." is not defined !")
        os.exit(1)
    end
    return value
end

-- local copy command --
CP = ""
if os.istarget("macosx") then
    CP= "cp -r "
elseif os.istarget("windows") then
    CP= convertPath(checkAndGetEnv("DIR_TOOLS_ROOT").."\\cp -ru ")
else
    CP="cp -ru "
end

-- local make directory command --
MKDIR=""
if os.istarget("windows") then
    MKDIR = convertPath(checkAndGetEnv("DIR_TOOLS_ROOT").."\\mkdir -p ")
else
    MKDIR = "mkdir -p "
end

-- local zip command --
ZIP = ""
if os.istarget("macosx") then
    ZIP= "zip -j9 -r "
elseif os.istarget("windows") then
    ZIP= convertPath(checkAndGetEnv("DIR_TOOLS_ROOT").."\\zip -j9 -r ")
else
    ZIP="zip -j9 -r "
end

-- function to get last directory name for objects and binaries
function getLastDirName(cfg,pltf)

    local dirname = iif(os.istarget("windows"),"win","linux")
    dirname = dirname.."32"
    if (os.istarget("windows")) then
        if (not string.startswith(_ACTION,"vs")) then
           dirname = dirname.."-gcc"
        end
    end
    
    if (string.startswith(cfg,"Debug")) then
       dirname = dirname.."-dbg"
    elseif (string.startswith(cfg,"ReleaseWithSymbols")) then
       dirname = dirname.."-sym"
    end

    if string.find(pltf,"x64") then
      dirname = string.gsub(dirname,"32","64")
    end 
    return dirname
end

-- function to get static lib extensions
function getStaticLibExtension()
   local ext = iif(string.startswith(_ACTION,"vs"), ".lib", ".a")
   return ext
end

-- function to get shared lib extensions
function getSharedLibExtension()
   local ext = iif(os.istarget("windows"), ".dll", ".so")
   return ext
end

-- function to get object files extensions
function getObjectExtension()
   local ext = iif(string.startswith(_ACTION,"vs"), ".obj", ".o")
   return ext
end



function SolutionConfiguration()
    -- configurations {"Release","Debug","ReleaseWithSymbols"}
    configurations {"Release","Debug"}
    platforms { "x64", "x32" }
    basedir "."
    os.chdir(workspace().basedir)
    location("build")
    language "C++"
end

-- function to append misc options to project
function FilterCommonOptions()
    filter {}

    local _projectRoot =  path.getabsolute(".")
    
    undefines { "UNICODE" }
    includedirs {convertPath(_projectRoot.."/include")}
    includedirs {checkAndGetEnv("DIR_INCL_ROOT")}
    objdir(convertPath(_projectRoot.."/build/obj"))

    prebuildcommands (MKDIR..convertPath(_projectRoot.."/bin/"))
    prebuildcommands (MKDIR..convertPath(_projectRoot.."/lib/"))
    prebuildcommands (MKDIR..convertPath(checkAndGetEnv("DIR_BIN_ROOT")))
    prebuildcommands (MKDIR..convertPath(checkAndGetEnv("DIR_LIB_ROOT")))
    prebuildcommands (MKDIR..convertPath(checkAndGetEnv("DIR_INCL_ROOT")))
    
    if (os.istarget("windows")) then
        defines { "WIN32", "_WIN32", "_WINDOWS" }
        includedirs {checkAndGetEnv("DIR_BOOST_ROOT")}
        if (string.startswith(_ACTION, "vs")) then
            if string.startswith(_ACTION, "vs2010") then
                --toolset "v100"
            else
                --toolset "v100"
                --toolset "v140"
            end
            defines {"_CRT_SECURE_NO_WARNINGS"}
            links { "netapi32"}
            linkoptions {"-NODEFAULTLIB:LIBCD.lib","-NODEFAULTLIB:LIBC.lib","-NODEFAULTLIB:LIBCMT.lib"}
            filter "configurations:Release"
                defines {"_CRT_NONSTDC_NO_DEPRECATE"}
        else
            links { "libnetapi32"}
        end
        if (string.startswith(_ACTION, "vs")) then
            filter "configurations:Debug"
                flags {"NoManifest"}
        end
   else
      defines { "__UNIX__" }
      includedirs {"/usr/include/c++/"..checkAndGetEnv("CPP_VER")}
      includedirs {"/usr/include/x86_64-linux-gnu/c++/"..checkAndGetEnv("CPP_VER") }
      includedirs {"/usr/include/libxml2"}
      includedirs {checkAndGetEnv("DIR_BOOST_ROOT")}
   end
   
   
    defines {"_MBCS"}
   filter {"system:windows", "action:vs*"}
        toolset "v100"
   filter {"system:windows", "action:vs*", "platforms:x64"}
        toolset "msc-Windows7.1SDK"
     
    filter "configurations:Debug"
        defines { "DEBUG", "_DEBUG" }
        symbols "On"
    
    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "Speed"
    
    filter "configurations:ReleaseWithSymbols"
        defines { "NDEBUG" }
        symbols "On"
       
    filter {}
end

function LicenseOptions()
    files {os.getenv("DIR_NGOSERIAL_ROOT").."/include/NgoLicense.h"}
    includedirs {os.getenv("DIR_NGOSERIAL_ROOT").."/include"}
    includedirs {os.getenv("DIR_NGOSERIAL_ROOT").."/rlm/src"}
    
    if os.istarget('windows') then 
        links { 
            "NgoRlmClient",
            "ws2_32",
            "wbemuuid", 
            -- "legacy_stdio_definitions"   FOR vs2017 toolset
        }
        -- files { os.getenv("DIR_NGOSERIAL_ROOT").."/rlm/iob.cpp"}  FOR vs2017 toolset
    end
    links { "boost_filesystem", "boost_regex"}
        
    sln = solution()
    for _, cfg in ipairs(sln.configurations) do
        for _, pltf in ipairs(sln.platforms) do
            filter {"configurations:"..cfg, "platforms:"..pltf}
            local _lastdirname = getLastDirName(cfg,pltf)
            libdirs(os.getenv("DIR_NGOSERIAL_ROOT").."/lib/".._lastdirname,
                    os.getenv("DIR_NGOSERIAL_ROOT").."/lib/cryptolicensing")
        end
    end
    filter {}
end

function PrefilterSharedLibBuildOptions(_targetname)
    kind "SharedLib"
    targetname(_targetname)
    files {"include/**.h", "src/**.cpp", "src/**.c"}

    if (os.istarget("windows")) then
        files { "src/**VERSIONINFO.rc" }
    end
    -- files { "version.cpp" }
end

function FilterSharedLibBuildOptions(_targetname)
    FilterCommonOptions()
    
    sln = workspace()
    local _rootdir =  path.getabsolute(".")
    for _, cfg in ipairs(sln.configurations) do
        for _, pltf in ipairs(sln.platforms) do
            local _lastdirname = getLastDirName(cfg,pltf)

            filter {"kind:SharedLib", "configurations:"..cfg, "platforms:"..pltf}
                libdirs { convertPath(path.join(checkAndGetEnv("DIR_LIB_ROOT"),_lastdirname ))}
                if (os.istarget("windows")) then
                    targetdir(convertPath(_rootdir.."/bin/".._lastdirname ))
                    implibdir(convertPath(_rootdir.."/lib/".._lastdirname ))
                else
                    targetdir(convertPath(_rootdir.."/lib/".._lastdirname ))
                end
                prebuildcommands (MKDIR..convertPath(_rootdir.."/bin/".._lastdirname ))
                prebuildcommands (MKDIR..convertPath(_rootdir.."/lib/".._lastdirname ))
                postbuildcommands(MKDIR..convertPath(checkAndGetEnv("DIR_BIN_ROOT").."/".._lastdirname ))
                postbuildcommands(MKDIR..convertPath(checkAndGetEnv("DIR_LIB_ROOT").."/".._lastdirname ))
                if (os.istarget("windows")) then
                    postbuildcommands(CP..convertPath(_rootdir.."/bin/".._lastdirname.."/*" .._targetname)..getSharedLibExtension().." "..convertPath(checkAndGetEnv("DIR_BIN_ROOT").."/".._lastdirname))
                    postbuildcommands(CP..convertPath(_rootdir.."/lib/".._lastdirname.."/*".._targetname)..getStaticLibExtension().." "..convertPath(checkAndGetEnv("DIR_LIB_ROOT").."/".._lastdirname))
                else
                    postbuildcommands(CP..convertPath(_rootdir.."/lib/".._lastdirname.."/*" .._targetname)..getSharedLibExtension().." "..convertPath(checkAndGetEnv("DIR_LIB_ROOT").."/".._lastdirname))
                end
                postbuildcommands(CP..convertPath(_rootdir.."/include/*").." "..convertPath(checkAndGetEnv("DIR_INCL_ROOT")))
                if string.find(_lastdirname,"win%d%d.dbg") then
                    postbuildcommands(CP..convertPath(_rootdir.."/bin/".._lastdirname.."/" .._targetname)..".pdb".." "..convertPath(checkAndGetEnv("DIR_BIN_ROOT").."/".._lastdirname))
                end
        end
    end
end

function PrefilterStaticLibBuildOptions(_targetname)
        kind "StaticLib"
        targetname(_targetname)
        files {"include/**.h", "src/**.cpp", "src/**.c"}
end

function FilterStaticLibBuildOptions(_targetname)
    FilterCommonOptions()

    sln = workspace()
    local _rootdir =  path.getabsolute(".")
    for _, cfg in ipairs(sln.configurations) do
        for _, pltf in ipairs(sln.platforms) do
            local _lastdirname = getLastDirName(cfg,pltf)
            filter {"kind:StaticLib", "configurations:"..cfg, "platforms:"..pltf}
                libdirs { convertPath(path.join(checkAndGetEnv("DIR_LIB_ROOT"),_lastdirname))}
                targetdir(convertPath(_rootdir.."/lib/".._lastdirname))
                prebuildcommands (MKDIR..convertPath(_rootdir.."/lib/".._lastdirname))
                postbuildcommands(MKDIR..convertPath(checkAndGetEnv("DIR_LIB_ROOT").."/".._lastdirname))
                postbuildcommands(CP..convertPath(_rootdir.."/lib/".._lastdirname.."/*".._targetname)..getStaticLibExtension().." "..convertPath(checkAndGetEnv("DIR_LIB_ROOT").."/".._lastdirname))
                postbuildcommands(CP..convertPath(_rootdir.."/include").." "..convertPath(checkAndGetEnv("DIR_INCL_ROOT")))
        end
    end
end

function PrefilterExeBuildOptions(_targetname)
        kind "ConsoleApp"
        targetname(_targetname)
end

function FilterExeBuildOptions(_targetname)
    FilterCommonOptions()
    
    sln = workspace()
    for _, cfg in ipairs(sln.configurations) do
        for _, pltf in ipairs(sln.platforms) do
            local _lastdirname = getLastDirName(cfg,pltf)
            filter {"kind:ConsoleApp", "configurations:"..cfg, "platforms:"..pltf}
                libdirs {convertPath(checkAndGetEnv("DIR_LIB_ROOT").."/".._lastdirname)}
                targetdir(convertPath(checkAndGetEnv("DIR_BIN_ROOT").."/".._lastdirname))
        end
    end
end

function PrefilterTestBuildOptions(_targetname)
    kind "ConsoleApp"
    files {"test/**.cpp"}
    links {"UnitTest", "NgoErr"}
    includedirs {checkAndGetEnv("DIR_UNITTEST_ROOT").."/src"}
    targetname(_targetname)
end

function FilterTestBuildOptions(_targetname)
    FilterExeBuildOptions(_targetname)    

    sln = workspace()
    local _rootdir =  path.getabsolute(".")
    for _, cfg in ipairs(sln.configurations) do
        for _, pltf in ipairs(sln.platforms) do
            local _lastdirname = getLastDirName(cfg,pltf)
            filter {"kind:ConsoleApp", "configurations:"..cfg, "platforms:"..pltf}
                libdirs{ convertPath(checkAndGetEnv("DIR_UNITTEST_ROOT").."/lib/".._lastdirname )}
                targetdir(convertPath(_rootdir.."/bin/".._lastdirname))
                debugdir(convertPath(checkAndGetEnv("DIR_BIN_ROOT").."/".._lastdirname))
        end
    end
end
