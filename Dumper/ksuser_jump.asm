
;
; created by AheadLibEx
; Author:i1tao
; Blog:https://www.cnblogs.com/0xc5
; Github:https://github.com/i1tao/AheadLibEx
;
;
; �� .asm �ļ���ӵ�����
; �Ҽ������ļ�-����-����
; ������:�Զ������ɹ���
; ���������ų�:��
;
; Ȼ����������������
; ������: ml64 /Fo $(IntDir)%(fileName).obj /c /Cp %(fileName).asm
; ���: $(IntDir)%(fileName).obj;%(Outputs)
; ���Ӷ���: ��
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
