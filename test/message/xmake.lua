-- test/message/xmake.lua

target("test_message")
    set_kind("binary")

    add_files("test_message.c")
    add_files("../../app/message/*.c")
    add_includedirs("../../app")
    
    add_packages("cjson", "log.c")
