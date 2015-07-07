rem ################################
rem # This file should be called from CCS directly
rem ###############################

setlocal

@echo %cd%
cd ../utils/tiobj2bin
@echo %cd%
@echo off
set binfolder=%cd%
@echo on
@echo %binfolder%
@echo off
set binexe=%binfolder%\tiobj2bin
cd ..\..\tools\compiler\*ti-cgt*\bin
set compilerbin=%cd%
@echo on
@echo %binexe%
@echo %compilerbin%
@cd /D %~dp0
@echo %cd%
@echo off
set outfolder=%cd%\Debug\
cd %outfolder%
@echo on
@echo %outfolder%
@echo.
@echo.
@echo Searching for .out files in Debug folder

@echo off
for /r %%i in (*.out) DO (
	 @echo Found out file %%~ni%%~xi
	 @echo Generating bin file %%~ni.bin
	 call %binexe% "%%i" "%%~ni.bin" "%compilerbin%\armofd.exe" "%compilerbin%\armhex.exe" "%binfolder%\mkhex4bin.exe"
)
@pause
