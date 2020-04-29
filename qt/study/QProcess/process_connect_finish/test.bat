@echo off
set num=0
:loop	
set /a num+=1
echo num = %num%
ping -n 2 www.baidu.com > nul
if "%num%"=="10" goto end
goto loop
:end
echo end of test