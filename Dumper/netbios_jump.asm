
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
EXTERN pfnAheadLibEx_Netbios:dq;

.CODE
AheadLibEx_Netbios PROC
	jmp pfnAheadLibEx_Netbios
AheadLibEx_Netbios ENDP


END
