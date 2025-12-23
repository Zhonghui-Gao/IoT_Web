-- test/xx/xmake.lua

target("demo")
    set_kind("binary")
    
    add_files("test.c")
    -- add_includedirs("../app")
    add_packages("cjson", "log.c")
