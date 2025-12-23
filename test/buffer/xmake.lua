-- test/buffer/xmake.lua

target("test_buffer")
    set_kind("binary")
    
    add_files("test_buffer.c")
    add_files("../../app/buffer/*.c")
    add_includedirs("../../app")
    
    add_packages("cjson", "log.c")
