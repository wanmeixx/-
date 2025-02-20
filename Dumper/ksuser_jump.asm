
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
EXTERN pfnAheadLibEx_KsCreateAllocator:dq;
EXTERN pfnAheadLibEx_KsCreateAllocator2:dq;
EXTERN pfnAheadLibEx_KsCreateClock:dq;
EXTERN pfnAheadLibEx_KsCreateClock2:dq;
EXTERN pfnAheadLibEx_KsCreatePin:dq;
EXTERN pfnAheadLibEx_KsCreatePin2:dq;
EXTERN pfnAheadLibEx_KsCreateTopologyNode:dq;
EXTERN pfnAheadLibEx_KsCreateTopologyNode2:dq;

.CODE
AheadLibEx_KsCreateAllocator PROC
	jmp pfnAheadLibEx_KsCreateAllocator
AheadLibEx_KsCreateAllocator ENDP

AheadLibEx_KsCreateAllocator2 PROC
	jmp pfnAheadLibEx_KsCreateAllocator2
AheadLibEx_KsCreateAllocator2 ENDP

AheadLibEx_KsCreateClock PROC
	jmp pfnAheadLibEx_KsCreateClock
AheadLibEx_KsCreateClock ENDP

AheadLibEx_KsCreateClock2 PROC
	jmp pfnAheadLibEx_KsCreateClock2
AheadLibEx_KsCreateClock2 ENDP

AheadLibEx_KsCreatePin PROC
	jmp pfnAheadLibEx_KsCreatePin
AheadLibEx_KsCreatePin ENDP

AheadLibEx_KsCreatePin2 PROC
	jmp pfnAheadLibEx_KsCreatePin2
AheadLibEx_KsCreatePin2 ENDP

AheadLibEx_KsCreateTopologyNode PROC
	jmp pfnAheadLibEx_KsCreateTopologyNode
AheadLibEx_KsCreateTopologyNode ENDP

AheadLibEx_KsCreateTopologyNode2 PROC
	jmp pfnAheadLibEx_KsCreateTopologyNode2
AheadLibEx_KsCreateTopologyNode2 ENDP


END
