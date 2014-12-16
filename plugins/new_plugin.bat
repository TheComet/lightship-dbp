@ECHO OFF
if "%1" == "" echo Please set a plugin name with "new_plugin.bat my_plugin_name". MUST be all lower-case and have no dashes.
if "%1" == "" goto failure
cmake "-DNAME=%1" "plugin_template\"

del "ALL_BUILD*"
del "ZERO_CHECK*"
del "*.cmake"
del "CMakeCache.txt"
del "Project*"
rmdir /S /Q "CMakeFiles"

:failure