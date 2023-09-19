@echo off
cd %~dp0%
setlocal EnableDelayedExpansion
set uv=C:\Keil_v5\UV4\UV4.exe
set prj_dir=F:\manage_tool\keil_build\rabbit\prj\examples
set uv_prj_path=
set file=
set rows=0
set /a line=1
set /a last=0
set filepath=
for /f "tokens=1,2 delims=:" %%b in (device.txt) do (
	echo %%b
	set filepath=%~dp0%%%b
	for /f "delims=" %%a in ('dir /ad/b %prj_dir%') do (
		echo %%c | findstr /L "%%a" > nul && set flag=1 || set flag=0
		if !flag! == 0 (
			echo bulding %%a
			if not exist "%~dp0%%%b\" (
				md "%~dp0%%%b\"
			)
			set file=!filepath!\%%a_build_log.txt
			echo !file!
			set uv_prj_path=%prj_dir%\%%a
			pushd !uv_prj_path!
			for /f "delims=" %%i in ('dir /a/b/s *.uvprojx') do (
				set uv_prj_path=%%i
			)
			popd
			echo !uv_prj_path!
			%uv% !uv_prj_path! -n %%b
			echo "***************Compile project**************"
			%uv% -j0 -r !uv_prj_path! -o !file!
			for /f "tokens=1 delims=~" %%j in (!file!) do set /a rows+=1
			set /a last=!rows! - %line%
			echo "-------------------------------------------"
			more +%last% !file!
			echo "-------------------------------------------"
		) else (
			echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
			echo %%b not support %%a
			echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
		)
	)
)
pause