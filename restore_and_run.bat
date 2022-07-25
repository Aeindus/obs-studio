@echo off

rem Restore OBS config and run


set backup_path=%appdata%\obs-studio\backup
set obs_data_path=%appdata%\obs-studio\basic

if "%1"=="/?" goto :help
if "%1"=="/backup" goto create_backup
goto restore_and_run

:help
	echo.
	echo 	[]		Restores the backup and starts the program
	echo 	[/backup] 	Replaces/Creates a backup
	exit
:create_backup
	rmdir /S /Q %backup_path%
	if exist "%backup_path%" (
		echo Backup delete failed! Most likely the files in backup folder are in use.
		timeout 5
	)
	xcopy "%obs_data_path%\*.*" "%backup_path%\"  /E /Y /R /I
	if not exist "%backup_path%\scenes\Untitled.json" (
		echo Backup failed! No scenes\Untitled.json file was found in backup.
		timeout 5
	)
	exit
	
:restore_and_run
	call :restore
	call :run
	exit /b 0

:restore
	if not exist "%backup_path%" (
		echo No backup folder was found
		timeout 4
		exit /b 
	)
	if not exist "%backup_path%\scenes\Untitled.json" (
		echo No backup scenes\Untitled.json was found
		timeout 4
		exit /b
	) 
	xcopy "%backup_path%\*.*" "%obs_data_path%\" /E /Y /R /I
	exit /b
	
:run
	start bin\64bit\obs64.exe
	exit
