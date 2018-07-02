
dofile (path.join(os.getenv("DIR_SCRIPTS_ROOT"),"premake5_common.lua"))

workspace "NgoUnit" 

    SolutionConfiguration()
    defines {
        "NGOLIBXML2_USE_DYN",
        "NGOERR_USE_DYN",
        "MU_PSR_USE_DYN",
        "NGOCOM_USE_DYN",
        "XMLMGR_USE_DYN",
        "NGOUNIT_USE_DYN"
    }
    local _exportSymbol = "NGOUNIT_MAKE_DLL"
    links { 
        "NgoLibxml2",
        "NgoErr",
        "muParser",
        "NgoCommon",
        "NgoXmlMgr"
    }
    
    -- PROTECTED REGION ID(NgoUnit.premake.solution) ENABLED START
    -- Insert here user code

    -- End of user code
    -- PROTECTED REGION END

project "NgoUnit"

    PrefilterSharedLibBuildOptions("NgoUnit")
    defines {_exportSymbol}
    
   -- PROTECTED REGION ID(NgoUnit.premake.sharedlib) ENABLED START
   -- Insert here user code

   -- End of user code
   -- PROTECTED REGION END

    FilterSharedLibBuildOptions("NgoUnit")


    
