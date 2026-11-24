# Locale: zh-hans (简体中文)
# 变量格式: G_{Level}{Component}{Number}
#   Level    = I | D | W | E (Info / Doc / Warning / Error)
#   Component= 组件全名 (Functions, CommonSettings, Dependencies,
#              GoogleTest, Libdeflate, ...) — 不允许单字母简写,避免冲突
#   Number   = 十进制序号
# --------------------------------------------------------------


# ---- CMakeLists.txt 选项说明 ----
set(G_DC1 "启用三方依赖缓存")

# ---- CommonSettings ----
set(G_ICommonSettings0 "使用'{0}'，位于'{1}'")

# ---- Dependencies ----
set(G_EDependencies0 "ensure_package需要NAME, REPO, TAG, INSTALL_PREFIX")
set(G_IDependencies1 "找到'{0}'（系统或{1}）")
set(G_IDependencies2 "未找到'{0}'。从'{1}'获取...")

# ---- Functions ----
set(G_EFunctions0 "源码目录'{0}'不存在")
set(G_IFunctions3 "目标'{0}'缓存命中（哈希'{1}'）")
set(G_IFunctions4 "目标'{0}'缓存未命中，从源代码构建（期望哈希'{1}'）")
set(G_DFunctions5 "将'{0}'的缓存更新为'{1}'")
set(G_EFunctions6 "文件'{0}'不存在")
set(G_DFunctions7 "已删除过时缓存'{0}'")

# ---- GoogleTest ----
set(G_DGoogleTest0 "如果找不到GoogleTest，则自动下载并构建GoogleTest。")
set(G_DGoogleTest1 "GoogleTest自动安装的Git标签")
set(G_DGoogleTest2 "自动构建GoogleTest的安装路径前缀")
set(G_DGoogleTest3 "GoogleTest源码目录路径")
set(G_IGoogleTest0 "FindGoogleTest: 找到现有GoogleTest源码于'{0}'（跳过下载）")
set(G_IGoogleTest1 "FindGoogleTest: 未找到GoogleTest源码，从{0}下载...")
set(G_IGoogleTest2 "FindGoogleTest: GoogleTest构建成功")
set(G_IGoogleTest3 "FindGoogleTest: 未找到GoogleTest。如需启用自动下载，请设置-DGOOGLETEST_AUTO_INSTALL=ON")
set(G_IGoogleTest4 "FindGoogleTest: GoogleTest_INCLUDE_DIRS = '{0}'")
set(G_IGoogleTest5 "FindGoogleTest: GoogleTest_LIBRARIES = '{0}'")
set(G_IGoogleTest6 "FindGoogleTest: 版本 = '{0}'")
set(G_WGoogleTest0 "FindGoogleTest: 未能创建gtest目标。")

# ---- Libdeflate ----
set(G_DLibdeflate0 "如果找不到libdeflate，则自动下载并构建libdeflate。")
set(G_DLibdeflate1 "libdeflate自动安装的Git标签")
set(G_DLibdeflate2 "自动构建libdeflate的安装路径前缀")
set(G_DLibdeflate3 "将libdeflate构建为共享库而非静态库")
set(G_DLibdeflate4 "libdeflate源码目录路径")
set(G_ILibdeflate0 "FindLibdeflate: 找到现有libdeflate源码于'{0}'（跳过下载）")
set(G_ILibdeflate1 "FindLibdeflate: 未找到libdeflate源码，从{0}下载...")
set(G_ILibdeflate2 "FindLibdeflate: libdeflate构建成功")
set(G_ILibdeflate3 "FindLibdeflate: 未找到libdeflate。如需启用自动下载，请设置-DLIBDEFLATE_AUTO_INSTALL=ON")
set(G_ILibdeflate4 "FindLibdeflate: Libdeflate_INCLUDE_DIRS = '{0}'")
set(G_ILibdeflate5 "FindLibdeflate: Libdeflate_LIBRARIES = '{0}'")
set(G_ILibdeflate6 "FindLibdeflate: 版本 = '{0}'")
set(G_WLibdeflate0 "FindLibdeflate: 未能创建libdeflate目标。")

# ---- MinecraftData ----
set(G_DMinecraftData0 "如果找不到minecraft-data，则自动下载minecraft-data。")
set(G_DMinecraftData1 "minecraft-data自动下载的Git标签或提交")
set(G_DMinecraftData2 "自动下载minecraft-data的安装路径前缀")
set(G_DMinecraftData3 "要使用的Minecraft版本的协议数据（如1.21.11；留空则只提供数据根目录）")
set(G_DMinecraftData4 "minecraft-data源码目录路径")
set(G_IMinecraftData0 "FindMinecraftData: 找到现有minecraft-data源码于'{0}'（跳过下载）")
set(G_IMinecraftData1 "FindMinecraftData: 未找到minecraft-data源码，从{0}下载...")
set(G_IMinecraftData2 "FindMinecraftData: minecraft-data已就绪于'{0}'")
set(G_IMinecraftData3 "FindMinecraftData: 未找到minecraft-data。如需启用自动下载，请设置-DMINECRAFT_DATA_AUTO_INSTALL=ON")
set(G_IMinecraftData4 "FindMinecraftData: MinecraftData_DATA_DIR = '{0}'")
set(G_IMinecraftData5 "FindMinecraftData: MinecraftData_PC_DIR = '{0}'")
set(G_IMinecraftData6 "FindMinecraftData: MinecraftData_GAME_DIR = '{0}'")
set(G_IMinecraftData7 "FindMinecraftData: 数据修订 = '{0}'")
set(G_WMinecraftData0 "FindMinecraftData: 在'{0}'下未找到预期的minecraft-data结构（data/dataPaths.json与data/pc）。")
set(G_WMinecraftData1 "FindMinecraftData: 在data/pc中未找到Minecraft版本'{0}'。")
set(G_WMinecraftData2 "FindMinecraftData: Minecraft版本'{0}'缺少protocol.json。")
set(G_WMinecraftData3 "FindMinecraftData: Minecraft版本'{0}'看起来是被截断的数字。命令行请使用-DMINECRAFT_DATA_GAME_VERSION:STRING=<版本>。")
