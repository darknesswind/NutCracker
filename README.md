# fork from DamianXVI's squirrel decompiler
    
	
I found the source from http://cheatengine.org/forum/viewtopic.php?p=5477214&sid=00a71666ea27fea14168c6d158f69724 (http://www.mediafire.com/?ruulm32z36i5dff), and modify it for squirrel3 compatible.

最初版本的代码来自于 http://cheatengine.org/forum/viewtopic.php?p=5477214&sid=00a71666ea27fea14168c6d158f69724 (http://www.mediafire.com/?ruulm32z36i5dff), 为了能够兼容 squirrel3 生成的字节码简单修改了一下。

## PS:
This program work `only` on `standard` squirrel nut file.
Please note that squirrel is an open source script language, the user can modify the compiler result in their want.
The modify may include but not only: encrypt the string and bytecode, add a custom op code, ... ect.

本程序`仅`对`标准`的 squirrel 生成的 nut 文件有效。
而 squirrel 是一门开源的脚本语言，任何使用者都可以按照自己的需求修改其代码生成器。例如：对字节码进行加密、添加自定义的语法……等等

## PS2:
The file "SQuirrelCNut.bt" is a template can use in 010Editor that help you explore the file struct.

库中的 "SQuirrelCNut.bt" 文件是用于010Editor的squirrel的nut格式的模板，可以用来快速地确认一下文件的内容，或者研究文件格式。
