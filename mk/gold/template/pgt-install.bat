@echo off
rem Copies necessary program template files to the specified directory.

if "%~1"=="" (
echo %~nx0: Missing directory operand
echo Usage: %~nx0 DIRECTORY
echo     where DIRECTORY is Templates directory of your GOLD installation.
echo Example: %~nx0 "C:\Program Files\GOLD Parser Builder 5\Templates\"
exit /B
)

if not exist "%~f1\" (
echo Directory "%~f1" does not exist.
exit /B
)

@echo on

copy /Y %~dp0\main.pgt.mk    "%~f1\GNU Make - Mybuild - Main.pgt"
copy /Y %~dp0\tables.pgt.mk  "%~f1\GNU Make - Mybuild - Tables.pgt"
copy /Y %~dp0\symbols.pgt.mk "%~f1\GNU Make - Mybuild - Symbols.pgt"
copy /Y %~dp0\rules.pgt.mk   "%~f1\GNU Make - Mybuild - Rules.pgt"
