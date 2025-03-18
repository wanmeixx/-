
;
; created by AheadLibEx
; Author:i1tao
; Blog:https://www.cnblogs.com/0xc5
; Github:https://github.com/i1tao/AheadLibEx
;
;
; 把 .asm 文件添加到工程
; 右键单击文件-属性-常规
; 项类型:自定义生成工具
; 从生成中排除:否
;
; 然后复制下面命令填入
; 命令行: ml64 /Fo $(IntDir)%(fileName).obj /c /Cp %(fileName).asm
; 输出: $(IntDir)%(fileName).obj;%(Outputs)
; 链接对象: 是
;
; Add the .asm file into your project.
; Right click file -> Properties -> General
; Item Type : Custom Build Tool
; Excluded From Build : No
;
; In "Custom Build Tool" Properties
; Command Line :ml64 /Fo $(IntDir)%(fileName).obj /c /Cp %(fileName).asm
; Outputs:$(IntDir)%(fileName).obj;%(Outputs)
; Link Objects:Yes

.DATA
EXTERN pfnAheadLibEx_Netbios:dq;

.CODE
AheadLibEx_Netbios PROC
	jmp pfnAheadLibEx_Netbios
AheadLibEx_Netbios ENDP


END
