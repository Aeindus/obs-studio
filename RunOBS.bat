@echo off

REM Restore OBS config and run

set backup_path=%appdata%\obs-studio\backup
set obs_data_path=%appdata%\obs-studio\basic

if "%1"=="/?" goto help
if "%1"=="/backup" goto create_backup
if "%1"=="\backup" goto create_backup
if not "%1"=="" goto help
goto restore_and_run

:help
	echo.
	echo =============== Help ===============
	echo 	[]		Restores the backup and starts the program
	echo 	[/backup] 	Replaces/Creates a backup
	echo.
	exit
:create_backup
	echo.
	echo =============== Backup profiles ===============
	rmdir /S /Q %backup_path% > nul
	if exist "%backup_path%" (
		echo Backup delete failed! Most likely the files in backup folder are in use.
	)
	xcopy "%obs_data_path%\*.*" "%backup_path%\"  /E /Y /R /I
	
	echo.
	echo Backed up profiles:
	for %%i in ("%backup_path%\scenes\*.json") do (
		echo           %%~nxi
	)
	
	echo.
	pause
	exit
	
:restore_and_run
	echo.
	echo =============== Restore and Run ===============
	call :restore
	call :run
	exit /b 0

:restore
	if not exist "%backup_path%" (
		echo No backup folder was found
		pause
		exit /b 
	)
	xcopy "%backup_path%\*.*" "%obs_data_path%\" /E /Y /R /I
	
	echo.
	echo Restored profiles:
	for %%i in ("%backup_path%\scenes\*.json") do (
		echo           %%~nxi
	)
	
	echo.
	timeout 2
	exit /b
	
:run
	REM You must cd to the exe location otherwise it won't find libobs-d3d11
	REM and will error out with "Your GPU may not be suported"
	cd bin\64bit
	start obs64.exe
	exit
