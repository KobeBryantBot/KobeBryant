@echo off

cd %~dp0..
setlocal enabledelayedexpansion

set KOBE_BRYANT_SDK_REMOTE_PATH=https://github.com/KobeBryantBot/SDK-KobeBryant.git

@REM rem Process System Proxy
@REM for /f "tokens=3* delims= " %%i in ('Reg query "HKCU\Software\Microsoft\Windows\CurrentVersion\Internet Settings" /v ProxyEnable') do (
@REM     if %%i==0x1 (
@REM         echo [INFO] System Proxy enabled. Adapting Settings...
@REM         for /f "tokens=3* delims= " %%a in ('Reg query "HKCU\Software\Microsoft\Windows\CurrentVersion\Internet Settings" /v ProxyServer') do set PROXY_ADDR=%%a
@REM         set http_proxy=http://!PROXY_ADDR!
@REM         set https_proxy=http://!PROXY_ADDR!
@REM         echo [INFO] System Proxy enabled. Adapting Settings finished.
@REM         echo.
@REM     )
@REM )


echo [INFO] Fetching SDK-KobeBryant to GitHub ...
echo.

for /f "delims=" %%i in ('git rev-parse --abbrev-ref HEAD') do set KOBE_BRYANT_SDK_NOW_BRANCH=%%i
for /f "delims=" %%i in ('git describe --tags --always') do set KOBE_BRYANT_NOW_TAG_LONG=%%i
for /f "delims=-" %%i in ('git describe --tags --always') do set KOBE_BRYANT_NOW_TAG=%%i

echo KOBE_BRYANT_SDK_NOW_BRANCH %KOBE_BRYANT_SDK_NOW_BRANCH%
echo KOBE_BRYANT_NOW_TAG_LONG %KOBE_BRYANT_NOW_TAG_LONG%
echo KOBE_BRYANT_NOW_TAG %KOBE_BRYANT_NOW_TAG%
echo.

if not exist SDK-KobeBryant/header/ (
    echo [WARNING] SDK-KobeBryant files no found. Pulling from remote...
    echo.
    git clone %KOBE_BRYANT_SDK_REMOTE_PATH%
)

cd SDK-KobeBryant
git fetch --all
git reset --hard origin/%KOBE_BRYANT_SDK_NOW_BRANCH%
git checkout %KOBE_BRYANT_SDK_NOW_BRANCH%
cd ..

echo.
echo [INFO] Fetching SDK-KobeBryant to GitHub finished
echo.

@REM remove all directory except .git in SDK-KobeBryant
for /f "delims=" %%i in ('dir /b /ad SDK-KobeBryant') do (
    if not "%%i"==".git" (
        echo [INFO] Removing SDK-KobeBryant\%%i
        rd /s /q SDK-KobeBryant\%%i
    )
)

@REM copy all from output/sdk to SDK-KobeBryant
xcopy /e /y /i /q output\sdk\* SDK-KobeBryant

cd SDK-KobeBryant
for /f "delims=" %%i in ('git status . -s') do set KOBE_BRYANT_SDK_NOW_STATUS=%%i
if "%KOBE_BRYANT_SDK_NOW_STATUS%" neq "" (
    echo [INFO] Modified files found.
    echo.
    git add .
    if "%KOBE_BRYANT_SDK_NOW_BRANCH%" == "main" (
        git commit -m "From KobeBryant %KOBE_BRYANT_NOW_TAG%"
        if [%2] == [release] (
            git tag %KOBE_BRYANT_NOW_TAG%
        )
    ) else (
        git commit -m "From KobeBryant %KOBE_BRYANT_NOW_TAG_LONG%"
    )
    echo.
    echo [INFO] Pushing to origin...
    echo.
    if [%1] neq [action] (
        git push origin %KOBE_BRYANT_SDK_NOW_BRANCH%
        git push --tags origin %KOBE_BRYANT_SDK_NOW_BRANCH%
    ) else (
        git push git@github.com:KobeBryantBot/SDK-KobeBryant.git %KOBE_BRYANT_SDK_NOW_BRANCH%
        git push --tags git@github.com:KobeBryantBot/SDK-KobeBryant.git %KOBE_BRYANT_SDK_NOW_BRANCH%
    )
    cd ..
    echo.
    echo [INFO] Upload finished.
    echo.
    goto Finish
) else (
    cd ..
    echo.
    echo.
    echo [INFO] No modified files found.
    echo [INFO] No need to Upgrade SDK-KobeBryant.
    goto Finish
)

:Finish
if [%1]==[action] goto End
timeout /t 3 >nul
:End