
dofile (path.join(os.getenv("DirScriptsRoot"),"premake_common.lua"))

-- solution NgoUnit --
solution "NgoUnit"

    SolutionConfiguration()

    -- COMMON CONFIGURATION MODIFICATION - START --
    configuration {}
        -- common defines (adapt if necessary) --
       defines {
                "NGO_ERR_USE_DYN"
               ,"MU_PSR_USE_DYN"
               ,"NGO_COM_USE_DYN"
               ,"XMLMGR_USE_DYNAMIC"
               ,"NGO_UNIT_USE_DYN"
			   ,"STATIC_BUILD"
               }                
       -- for shared libs, export statement
       local _exportSymbol = "NGO_UNIT_MAKE_DLL"
       -- suffix to use for library versionning
       local _version = ""
       -- common libs  --
       links { 
                "NgoLibxml2"
               ,"NgoErr"
               ,"muParser"
               ,"NgoCommon"
               ,"NgoXmlMgr"
          }
    -- COMMON CONFIGURATION MODIFICATION - END --

project "NgoUnit"
    -- PROJECT MODIFICATIONS START--
    local _targetname = "NgoUnit"
    -- additional defines --
    defines {_exportSymbol}
    -- PROJECT MODIFICATIONS END--

    AppendSharedLibBuildOptions(_targetname.._version)

project "test_NgoUnit"
    -- PROJECT MODIFICATIONS START--
    local _targetname = "test_NgoUnit"
    links {"NgoUnit"}
    -- PROJECT MODIFICATIONS END--

    AppendTestBuildOptions(_targetname.._version)
