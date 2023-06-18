@ECHO off
SetLocal EnableDelayedExpansion
IF NOT EXIST bin mkdir bin
IF NOT EXIST bin\int mkdir bin\int

REM call vcvarsall.bat x64
SET cc=clang

REM ------------------
REM    Main Project
REM ------------------

REM ==============
REM Gets list of all C files
SET c_filenames= 
FOR %%f in (source\*.c) do SET c_filenames=!c_filenames! %%f
FOR %%f in (source\expr\*.c) do SET c_filenames=!c_filenames! %%f
REM ==============


SET backend=BACKEND_D3D11


REM ==============
if %cc% == cl.exe (
  SET compiler_flags=/Zc:preprocessor /wd4090 /wd5105 /nologo
  SET include_flags=/I.\source\ /I.\third_party\include\ /I.\third_party\source\
  SET linker_flags=/link /DEBUG:FULL /LIBPATH:.\third_party\lib OpenAL32.lib shell32.lib user32.lib winmm.lib userenv.lib gdi32.lib /LIBPATH:.\bin codebase.lib
  SET output=/Fe.\bin\eval /Fo.\bin\int\
  SET defines=/D_DEBUG /D_CRT_SECURE_NO_WARNINGS /D%backend%
)

if %cc% == clang (
  SET compiler_flags=-Wall -Wvarargs -Werror -Wno-unused-function -Wno-format-security -Wno-incompatible-pointer-types-discards-qualifiers -Wno-unused-but-set-variable -Wno-int-to-void-pointer-cast
  SET include_flags=-Isource -Ithird_party/include -Ithird_party/source
  SET linker_flags=-g -lcodebase -lshell32 -luser32 -lwinmm -luserenv -lgdi32 -lOpenAL32 -Lthird_party/lib -Lbin
  SET output=-obin/eval.exe
  SET defines=-D_DEBUG -D_CRT_SECURE_NO_WARNINGS -D%backend%
)
REM ==============

REM SET compiler_flags=!compiler_flags! -fsanitize=address

REM ==============
REM TODO(voxel): REMOVE BACKEND SPECIFIC LINKS
if %backend% == BACKEND_D3D11 (
  if %cc% == cl.exe (
    SET linker_flags=%linker_flags% dxguid.lib d3dcompiler.lib
  )
  if %cc% == clang (
    SET linker_flags=%linker_flags% -ldxguid -ld3dcompiler
  )
)
REM ==============


ECHO Building eval.exe...
%cc% %compiler_flags% %c_filenames% %defines% %include_flags% %output% %linker_flags%


REM ===================