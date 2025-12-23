-- test/task/xmake.lua

target("test_task")
    set_kind("binary")

    add_files("test_task.c")
    add_files("../../app/task/*.c")
    add_includedirs("../../app")
    
    add_packages("cjson", "log.c")
