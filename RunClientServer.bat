::Original Author: Jackblue https://gist.github.com/SirJackblue/dc9d8b7e001a994f17ee
::Modified Version: Ji_Rath
@echo off
setlocal ENABLEDELAYEDEXPANSION

:: OVERRIDES (Designed for binary versions which are hard to track, resolves typing in same directory on startup)

:: ex. "D:\Program Files\Epic Games\UE_5.0EA\Engine\Binaries\Win64\UnrealEditor.exe"
set EDITOR_LOCATION_OVERRIDE=""

:: You can edit arguments here (docs : https://docs.unrealengine.com/latest/INT/Programming/Basics/CommandLineArguments/index.html)
set "CLIENT_PARAMETERS= -game -log"
set "SERVER_PARAMETERS= -server -log"

:: END OVERRIDES

rem Find .uproject file
for /f "delims=" %%a in ('dir *.uproject /b') do set "PROJECT_NAME=%%a"

rem Check for overridden editor location
if NOT %EDITOR_LOCATION_OVERRIDE% == "" (
    echo Detected overridden Unreal Editor location
    set UE_PATH=%EDITOR_LOCATION_OVERRIDE%
    goto :RunUncooked
)

rem Get the Engine association from the uproject.
for /f "delims=" %%A in (' powershell -Command "(Get-Content %PROJECT_NAME% | ConvertFrom-Json).EngineAssociation" ') do set ENGINE_ASSOCIATION=%%A

echo Engine association for uproject is: %ENGINE_ASSOCIATION%

rem Get proper Editor exe name based on Unreal version
set MAJOR_VERSION=%ENGINE_ASSOCIATION:~0,1%
if "%MAJOR_VERSION%" == "5" (
    set "EDITOR_NAME=UnrealEditor.exe"
) else (
    set "EDITOR_NAME=UE4Editor.exe"
)

:FindEngine
rem Attempt to find Unreal Engine directory
setlocal ENABLEEXTENSIONS
set KEY_NAME="HKLM\SOFTWARE\EpicGames\Unreal Engine\%ENGINE_ASSOCIATION%"
set VALUE_NAME=InstalledDirectory
FOR /F "usebackq skip=2 tokens=1-2*" %%A IN (`REG QUERY %KEY_NAME% /v %VALUE_NAME% 2^>nul`) DO (
    set ValueName=%%A
    set ValueType=%%B
    set ValueValue=%%C
)

rem Check whether we were able to get the engine directory
if NOT defined ValueValue (

    rem Manual override for UE5 Early Access
    if "%ENGINE_ASSOCIATION%" == "5.0" (
        echo Unable to find Unreal Engine directory, attempting 5.0EA...
        set ENGINE_ASSOCIATION=5.0EA
        goto :FindEngine
    )

	echo Unable to find Unreal Engine directory, please enter location manually
	set /p ValueValue="Unreal Engine Directory: "
) else (
	echo Found Unreal Engine directory at: %ValueValue%
)

:FindEditor
set UE_PATH="%ValueValue%\Engine\Binaries\Win64\!EDITOR_NAME!"

rem Check whether the executable was able to be found
if NOT exist !UE_PATH! (
    @echo !UE_PATH! was not found, please enter name of Unreal Engine Editor executable
    set /p EDITOR_NAME="Executable Name: "
    goto :FindEditor
)

:RunUncooked

set PROJECT_PATH="%CD%\%PROJECT_NAME%"
set "SERVER_COMMAND=!UE_PATH! %PROJECT_PATH% %SERVER_PARAMETERS%"
set "CLIENT_COMMAND=!UE_PATH! %PROJECT_PATH% %CLIENT_PARAMETERS%"

:: Script
Title Starting %PROJECT_NAME%..

set /p LaunchMode="Launch mode [all(default)/server/client]:"
if "%LaunchMode%" == "server" goto :launchServer
if "%LaunchMode%" == "client" goto :launchClient
if  NOT "%LaunchMode%" == "all" if NOT "%LaunchMode%"=="" exit 
goto :launchAll

:launchServer
start "%PROJECT_NAME% - Dedicated Server" %SERVER_COMMAND%
exit

:launchClient
start "%PROJECT_NAME% - Client" %CLIENT_COMMAND%
exit

:launchAll
start "%PROJECT_NAME% - Dedicated Server" %SERVER_COMMAND%
start "%PROJECT_NAME% - Client" %CLIENT_COMMAND%
exit