@echo Create setup...
@echo off
d:\Qt\QtIFW-3.2.2\bin\binarycreator.exe -c .\config\config.xml -p .\packages .\release\FlyTerm1.0-win-x64_%date:~0,4%%date:~5,2%%date:~8,2%.exe
@echo Create setup OK
@echo Compress setup...
zip -rjq .\release\FlyTerm1.0-win-x64_%date:~0,4%%date:~5,2%%date:~8,2%.zip .\release\FlyTerm1.0-win-x64_%date:~0,4%%date:~5,2%%date:~8,2%.exe
@echo Compress setup OK.
del .\release\FlyTerm1.0-win-x64_%date:~0,4%%date:~5,2%%date:~8,2%.exe
pause