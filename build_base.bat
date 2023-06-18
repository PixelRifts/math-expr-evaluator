@ECHO off
SetLocal EnableDelayedExpansion
IF NOT EXIST bin mkdir bin
IF NOT EXIST bin\int mkdir bin\int
IF NOT EXIST bin\int\base mkdir bin\int\base
IF NOT EXIST bin\int\opt mkdir bin\int\opt
del /S /Q bin\int\opt\*
IF EXIST bin\codebase.lib del /Q bin\codebase.lib

REM call vcvarsall.bat x64
SET cc=clang

REM ------------------
REM      Options
REM ------------------

SET Use_Render2D=false
SET Use_Physics2D=false
SET Use_UI=false


SET backend=BACKEND_D3D11



REM ==============
if %cc% == cl.exe (
  SET compiler_flags=/Zc:preprocessor /wd4090 /wd5105 /nologo /DEBUG /c
  SET include_flags=/I.\source\ /I.\third_party\include\ /I.\third_party\source\
  SET output_flag=/Fo.\bin\int
  SET defines=/D_DEBUG /D_CRT_SECURE_NO_WARNINGS /D%backend%
  SET ar=lib.exe /out:bin\codebase.lib
)

if %cc% == clang (
  SET compiler_flags=-c -Wall -Wvarargs -Werror -Wno-unused-function -Wno-format-security -Wno-incompatible-pointer-types-discards-qualifiers -Wno-unused-but-set-variable -Wno-int-to-void-pointer-cast
  SET include_flags=-Isource -Ithird_party/include -Ithird_party/source
  SET output_flag=-obin/int
  SET defines=-D_DEBUG -D_CRT_SECURE_NO_WARNINGS -D%backend%
  SET ar=llvm-ar -rc bin/codebase.lib
)
REM ==============

REM SET compiler_flags=!compiler_flags! -fsanitize=address



REM ==============
REM optional layers

if %Use_Render2D% == true (
  ECHO Optional Layer Selected: Render2D
  SET c_filenames=!c_filenames! source\opt\render_2d.c
  %cc% %compiler_flags% source\opt\render_2d.c %defines% %include_flags% %output_flag%/opt/render_2d.obj
)

if %Use_Physics2D% == true (
  ECHO Optional Layer Selected: Physics2D
  SET c_filenames=!c_filenames! source\opt\phys_2d.c
  %cc% %compiler_flags% source\opt\phys_2d.c %defines% %include_flags% %output_flag%/opt/phys_2d.obj
)

if %Use_UI% == true (
  ECHO Optional Layer Selected: UI
  SET c_filenames=!c_filenames! source\opt\ui.c
  %cc% %compiler_flags% source\opt\ui.c %defines% %include_flags% %output_flag%/opt/ui.obj
)

REM ==============



REM ------------------
REM    Main Project
REM ------------------

REM ==============
FOR %%f in (source\base\*.c) do (
  %cc% %compiler_flags% %%~dpf/%%~nf.c %defines% %include_flags% %output_flag%/base/%%~nf.obj
)
FOR %%f in (source\impl\*.c) do (
  %cc% %compiler_flags% %%~dpf/%%~nf.c %defines% %include_flags% %output_flag%/base/%%~nf.obj
)
FOR %%f in (source\core\*.c) do (
  %cc% %compiler_flags% %%~dpf/%%~nf.c %defines% %include_flags% %output_flag%/base/%%~nf.obj
)
FOR %%f in (source\os\*.c) do (
  %cc% %compiler_flags% %%~dpf/%%~nf.c %defines% %include_flags% %output_flag%/base/%%~nf.obj
)
REM ==============

REM Gets list of all C files
SET obj_filenames=
FOR %%f in (bin\int\base\*.obj) do SET obj_filenames=!obj_filenames! %%f
FOR %%f in (bin\int\opt\*.obj) do SET obj_filenames=!obj_filenames! %%f

ECHO Building codebase.lib...
%ar% %obj_filenames%
