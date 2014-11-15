@ECHO OFF
     
set DEV=C:\DEVEL
set QTDIR=C:\Qt\4.8.6
set PATH=%SystemRoot%;%SystemRoot%\system32;C:\Program Files (x86)\CMake\bin;%QTDIR%\bin
     
echo Setting OpenSSL Env.
set OPENSSL=C:\OpenSSL-Win32
set PATH=%OPENSSL%\bin;%PATH%
set LIB=%OPENSSL%\lib
set INCLUDE=%OPENSSL%\include
     
rem echo Setting NASM Env.
rem set PATH=%DEV%\NASM;%PATH%
     
echo Setting DirectX Env.
rem set LIB=%DEV%\DXSDK2010\Lib\x86;%LIB%
rem set INCLUDE=%DEV%\DXSDK2010\Include;%INCLUDE%
     
echo Setting MSVC2010 Env.
set VSINSTALLDIR=C:\Program Files (x86)\Microsoft Visual Studio 10.0
set VCINSTALLDIR=C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC
set DevEnvDir=%VSINSTALLDIR%\Common7\IDE
set PATH=%VCINSTALLDIR%\bin;%VSINSTALLDIR%\Common7\Tools;%VSINSTALLDIR%\Common7\IDE;%VCINSTALLDIR%\VCPackages;%PATH%
set INCLUDE=%VCINSTALLDIR%\include;%INCLUDE%
set LIB=%VCINSTALLDIR%\lib;%LIB%
set LIBPATH=%VCINSTALLDIR%\lib

echo Setting Windows SDK Env.
set WindowsSdkDir=C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A
set PATH=%WindowsSdkDir%\Bin;%PATH%
set LIB=%WindowsSdkDir%\Lib;%LIB%
set INCLUDE=%WindowsSdkDir%\Include;%INCLUDE%
set TARGET_CPU=x86
     
echo Setting Framework Env.
set FrameworkVersion=v4.0.30319
set Framework35Version=v3.5
set FrameworkDir=%SystemRoot%\Microsoft.NET\Framework
set LIBPATH=%FrameworkDir%\%FrameworkVersion%;%FrameworkDir%\%Framework35Version%;%LIBPATH%
set PATH=%LIBPATH%;%PATH%;D:\Perl\bin
     
echo Env. ready.
     
