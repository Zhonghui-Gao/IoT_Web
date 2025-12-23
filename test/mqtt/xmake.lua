-- test/mqtt/xmake.lua

target("test_mqtt")
    set_kind("binary")

    add_files("test_mqtt.c")
    add_files("../../app/mqtt/*.c")
    add_includedirs("../../app")
    
    add_packages("cjson", "log.c")
