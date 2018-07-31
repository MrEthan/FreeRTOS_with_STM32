SET PATH=F:\software\Keil_v5\ARM\ARMCC\Bin;C:\ProgramData\Oracle\Java\javapath;C:\UserSoftware\Python34\;C:\UserSoftware\Python34\Scripts;C:\UserSoftware\Java\jdk1.8.0_121\bin;C:\UserSoftware\Java\jdk1.8.0_121\jre\bin;C:\Users\xiaoming\.dnx\bin;C:\Program Files\Microsoft DNX\Dnvm\;C:\Program Files (x86)\Windows Kits\8.1\Windows Performance Toolkit\;C:\Program Files\Microsoft SQL Server\120\Tools\Binn\;C:\Program Files\Microsoft SQL Server\130\Tools\Binn\;C:\UserSoftware\Git\cmd;C:\UserSoftware\TortoiseGit\bin;c:\windows\system32;C:\UserSoftware\Git\;C:\Program Files\MySQL\MySQL Utilities 1.6\;C:\Program Files\MySQL\MySQL Server 5.7\bin;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\WINDOWS\System32\WindowsPowerShell\v1.0\;C:\WINDOWS\System32\OpenSSH\;C:\Users\xiaoming\AppData\Local\Microsoft\WindowsApps;
SET CPU_TYPE=STM32F103ZE
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=STM32F103ZE
SET CPU_CLOCK=0x00B71B00
"F:\software\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\led.__i"
"F:\software\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\usart.__i"
"F:\software\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\port.__i"
"F:\software\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\heap_2.__i"
"F:\software\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\queue.__i"
"F:\software\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\list.__i"
"F:\software\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\tasks.__i"
"F:\software\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\system.__i"
"F:\software\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\main.__i"
"F:\software\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\misc.__i"
"F:\software\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\stm32f10x_exti.__i"
"F:\software\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\stm32f10x_gpio.__i"
"F:\software\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\stm32f10x_rcc.__i"
"F:\software\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\stm32f10x_tim.__i"
"F:\software\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\stm32f10x_usart.__i"
"F:\software\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\stm32f10x_wwdg.__i"
"F:\software\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\gpio_stm32f10x.__i"
"F:\software\Keil_v5\ARM\ARMCC\Bin\ArmAsm" --Via ".\objects\startup_stm32f10x_hd._ia"
"F:\software\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\objects\system_stm32f10x.__i"
"F:\software\Keil_v5\ARM\ARMCC\Bin\ArmLink" --Via ".\Objects\FreeRTOS_V10.0.lnp"
"F:\software\Keil_v5\ARM\ARMCC\Bin\fromelf.exe" ".\Objects\FreeRTOS_V10.0.1_STM32" --i32combined --output ".\Objects\FreeRTOS_V10.0.hex"
