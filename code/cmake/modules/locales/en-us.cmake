# Locale: en-us (English)
# Variable format: G_{Level}{Component}{Number}
#   Level    = I | D | W | E (Info / (Doc/Debug) / Warning / Error)
#   Component= full component name (Functions, CommonSettings, Dependencies,
#              GoogleTest, Libdeflate, ...) — no single-letter abbreviations
#   Number   = decimal sequence
# --------------------------------------------------------------

# ---- CMakeLists.txt 选项说明 ----
set(G_DC0 "Enable ccache/sccache")
set(G_DC1 "Enable parallel builds")
set(G_DC2 "Enable artifact caching for components")
set(G_DC3 "Enable third-party dependency caching")
set(G_DC4 "Enable linked memory limit")
set(G_DC5 "Enable automatic link finder")

# ---- CommonSettings ----
set(G_ICommonSettings0 "Using '{0}', with '{1}'")

# ---- Dependencies ----
set(G_EDependencies0 "ensure_package requires NAME, REPO, TAG, INSTALL_PREFIX")
set(G_IDependencies1 "Found '{0}' (system or {1})")
set(G_IDependencies2 "'{0}' not found. Fetching from '{1}'...")

# ---- Functions ----
set(G_EFunctions0 "The source code directory '{0}' does not exist")
set(G_IFunctions3 "Target '{0}' cache hit (hash '{1}')")
set(G_IFunctions4 "Target '{0}' cache miss, building from source (expected hash '{1}')")
set(G_DFunctions5 "Update cache for '{0}' to '{1}'")
set(G_EFunctions6 "File '{0}' does not exist")
set(G_DFunctions7 "Removed stale cache '{0}'")

# ---- GoogleTest ----
set(G_DGoogleTest0 "Automatically download and build GoogleTest if not found.")
set(G_DGoogleTest1 "Git tag for GoogleTest auto-install")
set(G_DGoogleTest2 "Install prefix for auto-built GoogleTest")
set(G_DGoogleTest3 "GoogleTest source directory path")
set(G_IGoogleTest0 "FindGoogleTest: Found existing GoogleTest source at '{0}' (skip download)")
set(G_IGoogleTest1 "FindGoogleTest: GoogleTest source not found, downloading from {0}...")
set(G_IGoogleTest2 "FindGoogleTest: GoogleTest built successfully")
set(G_IGoogleTest3 "FindGoogleTest: GoogleTest not found. To enable automatic download, set -DGOOGLETEST_AUTO_INSTALL=ON")
set(G_IGoogleTest4 "FindGoogleTest: GoogleTest_INCLUDE_DIRS = '{0}'")
set(G_IGoogleTest5 "FindGoogleTest: GoogleTest_LIBRARIES = '{0}'")
set(G_IGoogleTest6 "FindGoogleTest: Version = '{0}'")
set(G_WGoogleTest0 "FindGoogleTest: Failed to create target gtest.")

# ---- Libdeflate ----
set(G_DLibdeflate0 "Automatically download and build libdeflate if not found.")
set(G_DLibdeflate1 "Git tag for libdeflate auto-install")
set(G_DLibdeflate2 "Install prefix for auto-built libdeflate")
set(G_DLibdeflate3 "Build libdeflate as a shared library instead of a static library")
set(G_DLibdeflate4 "libdeflate source directory path")
set(G_ILibdeflate0 "FindLibdeflate: Found existing libdeflate source at '{0}' (skip download)")
set(G_ILibdeflate1 "FindLibdeflate: libdeflate source not found, downloading {0}...")
set(G_ILibdeflate2 "FindLibdeflate: libdeflate built successfully")
set(G_ILibdeflate3 "FindLibdeflate: libdeflate not found. To enable automatic download, set -DLIBDEFLATE_AUTO_INSTALL=ON")
set(G_ILibdeflate4 "FindLibdeflate: Libdeflate_INCLUDE_DIRS = '{0}'")
set(G_ILibdeflate5 "FindLibdeflate: Libdeflate_LIBRARIES = '{0}'")
set(G_ILibdeflate6 "FindLibdeflate: Version = '{0}'")
set(G_WLibdeflate0 "FindLibdeflate: Failed to create a libdeflate target.")
