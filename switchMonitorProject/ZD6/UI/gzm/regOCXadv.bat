    @echo off
    if "%processor_architecture%"=="x86" goto REG32
    if "%processor_architecture%"=="AMD64" goto REG64
    :REG32
    if not exist %systemroot%\system32\TeeChart8.ocx COPY TeeChart8.ocx  %systemroot%\system32
    %systemroot%\system32\regsvr32.exe -u %systemroot%\system32\TeeChart8.ocx
    %systemroot%\system32\regsvr32.exe %systemroot%\system32\TeeChart8.ocx

    goto exit
    :REG64
    if not exist %systemroot%\syswow64\TeeChart8.ocx COPY TeeChart8.ocx  %systemroot%\syswow64
    %systemroot%\syswow64\regsvr32.exe -u %systemroot%\system32\TeeChart8.ocx
    %systemroot%\syswow64\regsvr32.exe %systemroot%\system32\TeeChart8.ocx

    goto exit

    :exit
	pause
    exit