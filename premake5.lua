dofile (path.join(os.getenv("DIR_SCRIPTS_ROOT"),"premake5_common.lua"))

workspace "NgoUnit"

    SolutionConfiguration()
    defines {
                "NGO_ERR_USE_DYN"
               ,"MU_PSR_USE_DYN"
               ,"NGO_COM_USE_DYN"
               ,"XMLMGR_USE_DYNAMIC"
               ,"NGO_UNIT_USE_DYN"
			   ,"STATIC_BUILD"
    }
    local _exportSymbol = "NGO_UNIT_MAKE_DLL"
    links { 
               "NgoErr"
               ,"muParser"
               ,"NgoCommon"
               ,"NgoXmlMgr"
    }
    
    -- PROTECTED REGION ID(NgoUnit.premake.solution) ENABLED START
	configuration {"vs2010","Release"}
			links {"NgoLibxml2"}
	configuration {"vs2010","Debug"}
			links {"NgoLibxml2"}
	configuration {"linux"}
			links {"xml2"}

    -- PROTECTED REGION END


project "NgoUnit"

    PrefilterSharedLibBuildOptions("NgoUnit")
    defines {_exportSymbol}
    
    -- PROTECTED REGION ID(NgoUnit.premake.sharedlib) ENABLED START

    -- PROTECTED REGION END

    FilterSharedLibBuildOptions("NgoUnit")


project "test_NgoUnit"

    PrefilterTestBuildOptions("test_NgoUnit")
    links { "NgoUnit"}
    
    -- PROTECTED REGION ID(NgoUnit.premake.test) ENABLED START

    -- PROTECTED REGION END

    FilterTestBuildOptions("test_NgoUnit")
