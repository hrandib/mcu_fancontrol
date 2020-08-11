$outFile = $args[0]

$elftool = "C:\Program Files (x86)\IAR Systems\Embedded Workbench 8.3\stm8\bin\ielftool.exe"
Start-Process $elftool "$outFile firmware.hex --ihex"

$flashtool = "c:\Program Files (x86)\STMicroelectronics\st_toolset\stvp\STVP_CmdLine.exe"
$flashargs = "-Device=STM8S003F3 -no_loop -FileProg=firmware.hex"
Start-Process $flashtool $flashargs