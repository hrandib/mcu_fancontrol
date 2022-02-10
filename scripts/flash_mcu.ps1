# Run configuration for QtCreator
# Executable:
#   Powershell
# Command line arguments:
#   C:/Projects/mcu_fancontrol/scripts/flash_mcu.ps1 %{ActiveProject:FileBaseName}.out
# Working directory:
#   %{buildDir}\install-root

$outFile = $args[0]
[Boolean]$writeEeepromFlag = $args[1] -or $False

$PSDefaultParameterValues['Out-File:Encoding'] = 'utf8'

$elftool = "C:\Program Files (x86)\IAR Systems\Embedded Workbench 8.3\stm8\bin\ielftool.exe"
& $elftool $outFile "firmware.hex" "--ihex"

#Split FLASH and EEPROM parts
$flashBeginContent = ":10800000"
$firmware = Get-Content -Raw firmware.hex

$splitIndex = $firmware.indexOf($flashBeginContent)
Set-Content -NoNewline -Path eeprom.hex -Value $firmware.Substring(0, $splitIndex)
Set-Content -NoNewline -Path flash.hex -Value $firmware.Substring($splitIndex)

$flashtool = "c:\Program Files (x86)\STMicroelectronics\st_toolset\stvp\STVP_CmdLine.exe"
$flashargs = @(
    "-Device=STM8S003F3",
    "-no_loop",
    "-FileProg=flash.hex"
    )
if ($writeEeepromFlag) {
    $flashargs += "-FileData=eeprom.hex"
}

& $flashtool $flashargs