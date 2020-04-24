@echo off
set num=0
:loop	
set /a num+=1
echo num = %num%
if "%num%"=="10" goto end
goto loop
:end
echo end of test