-- ./xmake.lua
set_project("IoT_Web")
set_version("1.0.0")

-- 设置语言标准
set_languages("c11")

-- 开启调试符号
add_rules("mode.debug", "mode.release")

add_repositories("my-repo https://github.com/Zhonghui-Gao/xmake-repo.git#feature/custom-package")

add_requires(
    "cjson",
    "log.c"
)

-- 添加链接库
add_links("m", "pthread", "paho-mqtt3c", "rt")

-- 包含子模块
-- includes("app")
includes("test/buffer")
includes("test/message")
includes("test/demo")
includes("test/test_task")
includes("test/mqtt")

-- ==========================================
-- 3. [新增] 配置根目录的 main.c
-- ==========================================
-- target("main_node")
--     set_kind("binary") -- 这是一个可执行程序
--     set_toolset("cc", "gcc")
--     set_toolset("ld", "gcc")
--     
--     -- 添加当前目录下的 main.c
--     -- add_files("main.c")
--     
--     -- 依赖核心库 app_core
--     -- 解释：
--     -- 1. 链接 app.a
--     -- 2. 自动获得 app/ 目录的头文件路径
--     -- 3. 自动获得 cjson 和 log 的库和头文件 (因为 app public了它们)
--     -- add_deps("app")