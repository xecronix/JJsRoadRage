@echo off
cd /d "%~dp0"
ctags.exe -R --totals --fields=fKsSt --extra=-fq --c-kinds=+p --sort=foldcase --excmd=number ^
  -f "C:\dev\JJsRoadRage\JJsRoadRageGeany.tags"
