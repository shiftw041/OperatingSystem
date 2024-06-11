;***********************************************************************************************************************
; os.asm
; 采用优先数调度算法实现任务切换，在屏幕同一位置处循环显示不同字符串
%include "pm.inc"	
; 4个任务页目录地址------------------------------------------------------------------------------------------------------
PageDirBase0		equ	200000h	; 页目录开始地址:	2M
PageTblBase0		equ	201000h	; 页表开始地址:		2M +  4K
PageDirBase1		equ	210000h	; 页目录开始地址:	2M + 64K
PageTblBase1		equ	211000h	; 页表开始地址:		2M + 64K + 4K
PageDirBase2		equ	220000h	; 页目录开始地址:	2M + 128K
PageTblBase2		equ	221000h	; 页表开始地址:		2M + 128K + 4K
PageDirBase3		equ	230000h	; 页目录开始地址:	2M + 192K
PageTblBase3		equ	231000h	; 页表开始地址:		2M + 192K + 4K

org 0100h			; 兼容FreeDos，留出100h的栈空间，以免栈溢出
    jmp LABEL_BEGIN

; ###################################################################################################
; 各项数据结构定义
; GDT定义----------------------------------------------------------------------------------------------------------------
[SECTION .gdt]
; GDT begin                        段基址,            段界限 , 属性
LABEL_GDT:			Descriptor	       0,                  0, 0								; 空描述符
LABEL_DESC_NORMAL:	Descriptor	       0,             0ffffh, DA_DRW						; Normal 描述符
LABEL_DESC_FLAT_C:	Descriptor         0,            0fffffh, DA_CR | DA_32 | DA_LIMIT_4K	; 0 ~ 4G
LABEL_DESC_FLAT_RW:	Descriptor         0,            0fffffh, DA_DRW | DA_LIMIT_4K			; 0 ~ 4G
LABEL_DESC_CODE32:	Descriptor	       0,   SegCode32Len - 1, DA_CR | DA_32					; 非一致代码段, 32位
LABEL_DESC_CODE16:	Descriptor	       0,             0ffffh, DA_C							; 非一致代码段, 16位
LABEL_DESC_DATA:	Descriptor	       0,	     DataLen - 1, DA_DRW						; Data
LABEL_DESC_STACK:	Descriptor	       0,         TopOfStack, DA_DRWA | DA_32				; Stack, 32位
LABEL_DESC_VIDEO:	Descriptor	 0B8000h,             0ffffh, DA_DRW + DA_DPL3				; 显存首地址

; TSS描述符---------------------------------------------------------------------------------------------------------------
LABEL_TASK0_DESC_TSS: 	Descriptor 		0,          TSS0Len-1, DA_386TSS	   ;TSS0
LABEL_TASK1_DESC_TSS: 	Descriptor 		0,          TSS1Len-1, DA_386TSS	   ;TSS1
LABEL_TASK2_DESC_TSS: 	Descriptor 		0,          TSS2Len-1, DA_386TSS	   ;TSS2
LABEL_TASK3_DESC_TSS: 	Descriptor 		0,          TSS3Len-1, DA_386TSS	   ;TSS3

; 四个任务进程段LDT-------------------------------------------------------------------------------------------------------
LABEL_TASK0_DESC_LDT:    Descriptor     0,    TASK0LDTLen - 1, DA_LDT
LABEL_TASK1_DESC_LDT:    Descriptor     0,    TASK1LDTLen - 1, DA_LDT
LABEL_TASK2_DESC_LDT:    Descriptor     0,    TASK2LDTLen - 1, DA_LDT
LABEL_TASK3_DESC_LDT:    Descriptor     0,    TASK3LDTLen - 1, DA_LDT
; GDT end
GdtLen		equ	$ - LABEL_GDT	; GDT长度
GdtPtr		dw	GdtLen - 1		; GDT界限
			dd	0				; GDT基地址

; GDT 各段选择子------------------------------------------------------------------------------------
SelectorNormal		equ	LABEL_DESC_NORMAL	- LABEL_GDT
SelectorFlatC		equ	LABEL_DESC_FLAT_C	- LABEL_GDT
SelectorFlatRW		equ	LABEL_DESC_FLAT_RW	- LABEL_GDT
SelectorCode32		equ	LABEL_DESC_CODE32	- LABEL_GDT	
SelectorCode16		equ	LABEL_DESC_CODE16	- LABEL_GDT	
SelectorData		equ	LABEL_DESC_DATA		- LABEL_GDT
SelectorStack		equ	LABEL_DESC_STACK	- LABEL_GDT
SelectorVideo		equ	LABEL_DESC_VIDEO	- LABEL_GDT
; 四个任务段选择子---------------------------------------------------------------------------------
SelectorTSS0        equ LABEL_TASK0_DESC_TSS    - LABEL_GDT
SelectorTSS1        equ LABEL_TASK1_DESC_TSS    - LABEL_GDT
SelectorTSS2        equ LABEL_TASK2_DESC_TSS    - LABEL_GDT
SelectorTSS3        equ LABEL_TASK3_DESC_TSS    - LABEL_GDT
SelectorLDT0        equ LABEL_TASK0_DESC_LDT   	- LABEL_GDT
SelectorLDT1        equ LABEL_TASK1_DESC_LDT    - LABEL_GDT
SelectorLDT2        equ LABEL_TASK2_DESC_LDT    - LABEL_GDT
SelectorLDT3        equ LABEL_TASK3_DESC_LDT 	- LABEL_GDT
; END of [SECTION .gdt]----------------------------------------------------------------------

; 定义任务------------------------------------------------------------------------------------
DefineTask 0, "VERY", 20, 0Ch	;黑底红字
DefineTask 1, "LOVE", 20, 0Fh	;黑底白字
DefineTask 2, "HUST", 20, 0Ch	;黑底红字
DefineTask 3, "MRSU", 20, 0Fh	;黑底白字
; END of 任务定义------------------------------------------------------------------------------------

; 数据段
; ---------------------------------------------------------------------------------------------
[SECTION .data1]	 ; 数据段
ALIGN	32			 ; 内存对齐32位
[BITS	32]			 ; 编译为32位代码
LABEL_DATA:
; 实模式下使用这些符号
; 字符串
_szPMMessage:		db	"In Protect Mode now. ^-^", 0Ah, 0Ah, 0	; 进入保护模式后显示此字符串
_szMemChkTitle:		db	"BaseAddrL BaseAddrH LengthLow LengthHigh   Type", 0Ah, 0	; 进入保护模式后显示此字符串
_szRAMSize			db	"RAM size:", 0
_szReturn			db	0Ah, 0
_szOwnerMessage:	db	"Shiftw-U202112131: Already Setup Paging!!", 0
; 变量
_wSPValueInRealMode	dw	0
_dwMCRNumber:		dd	0	; Memory Check Result
; 计算字符偏移量。显存中地址以字为单位即两个字节，所以要乘以2
_dwDispPos:			dd	(80 * 6 + 0) * 2	; 屏幕第 6 行, 第 0 列。
_dwMemSize:			dd	0
_ARDStruct:			; Address Range Descriptor Structure
_dwBaseAddrLow:		dd	0
_dwBaseAddrHigh:	dd	0
_dwLengthLow:		dd	0
_dwLengthHigh:		dd	0
_dwType:			dd	0
_PageTableNumber:	dd	0
_SavedIDTR:			dd	0	; 用于保存 IDTR
					dd	0
_SavedIMREG:		db	0	; 中断屏蔽寄存器值
_MemChkBuf:	 times	256	db	0

; 任务调度相关---------------------------------------------------------------
%define unitTimes   5	;设置单位时间片使程序运行结果更易于观察
_RunningTask:		dd	0
_TaskPriority:		dd	16*unitTimes, 10*unitTimes, 8*unitTimes, 6*unitTimes
_ExecutionTicks:	dd	0, 0, 0, 0	; 任务执行的时间片剩余
;---------------------------------------------------------------------------

; 保护模式下使用这些符号
szPMMessage		equ	_szPMMessage	- $$
szMemChkTitle	equ	_szMemChkTitle	- $$
szRAMSize		equ	_szRAMSize	- $$
szReturn		equ	_szReturn	- $$
dwDispPos		equ	_dwDispPos	- $$
dwMemSize		equ	_dwMemSize	- $$
ARDStruct		equ	_ARDStruct	- $$
dwBaseAddrLow	equ	_dwBaseAddrLow	- $$
dwBaseAddrHigh	equ	_dwBaseAddrHigh	- $$
dwLengthLow		equ	_dwLengthLow	- $$
dwLengthHigh	equ	_dwLengthHigh	- $$
dwType			equ	_dwType		- $$
MemChkBuf		equ	_MemChkBuf	- $$
SavedIDTR		equ	_SavedIDTR	- $$
SavedIMREG		equ	_SavedIMREG	- $$
PageTableNumber	equ	_PageTableNumber- $$
szOwnerMessage  equ _szOwnerMessage - $$
dwMCRNumber		equ	_dwMCRNumber	- $$
; 任务相关变量------------------------------------------
RunningTask     equ _RunningTask - $$
TaskPriority    equ _TaskPriority - $$
ExecutionTicks  equ _ExecutionTicks - $$
; -----------------------------------------------------
DataLen			equ	$ - LABEL_DATA		; 计算节长度
; END of [SECTION .data1]

; 全局堆栈段
[SECTION .gs]
ALIGN	32
[BITS	32]
LABEL_STACK:
	times 512 db 0

TopOfStack	equ	$ - LABEL_STACK - 1
; END of [SECTION .gs]

; IDT
[SECTION .idt]
ALIGN	32	
[BITS	32]	
LABEL_IDT:	; 定义中断门数组，用于处理不同中断
; 门                          目标选择子,            偏移, DCount, 属性
%rep 32
				Gate	SelectorCode32, SpuriousHandler,      0, DA_386IGate
%endrep
.020h:			Gate	SelectorCode32,    ClockHandler,      0, DA_386IGate
%rep 95
				Gate	SelectorCode32, SpuriousHandler,      0, DA_386IGate
%endrep
.080h:			Gate	SelectorCode32,  UserIntHandler,      0, DA_386IGate

IdtLen		equ	$ - LABEL_IDT	; IDT 长度
IdtPtr		dw	IdtLen - 1		; IDT 段界限
			dd	0				; IDT 基地址, 待设置
; END of [SECTION .idt]

; ###################################################################################################
; 16位代码段 实模式准备跳入保护模式
[SECTION .s16]
[BITS	16]
LABEL_BEGIN:
	; 准备工作
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, 0100h
	mov	[LABEL_GO_BACK_TO_REAL+3], ax
	mov	[_wSPValueInRealMode], sp
	; 得到内存数
	mov	ebx, 0
	mov	di, _MemChkBuf
.loop:
	mov	eax, 0E820h
	mov	ecx, 20
	mov	edx, 0534D4150h
	int	15h
	jc	LABEL_MEM_CHK_FAIL
	add	di, 20
	inc	dword [_dwMCRNumber]
	cmp	ebx, 0
	jne	.loop
	jmp	LABEL_MEM_CHK_OK
LABEL_MEM_CHK_FAIL:
	mov	dword [_dwMCRNumber], 0
LABEL_MEM_CHK_OK:

	; 初始化全局描述符
	InitDescBase LABEL_SEG_CODE16, LABEL_DESC_CODE16; 初始化16位代码段描述符
	InitDescBase LABEL_SEG_CODE32, LABEL_DESC_CODE32; 初始化32位代码段描述符
	InitDescBase LABEL_DATA, LABEL_DESC_DATA		; 初始化数据段
	InitDescBase LABEL_STACK, LABEL_DESC_STACK		; 初始化数据段
	; 初始化任务描述符------------------------------------------------------
	InitTaskDescBase 0
	InitTaskDescBase 1
	InitTaskDescBase 2
	InitTaskDescBase 3
	; ---------------------------------------------------------------------
	; 为加载 GDTR 作准备
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_GDT		; eax <- gdt 基地址
	mov	dword [GdtPtr + 2], eax	; [GdtPtr + 2] <- gdt 基地址

	; 为加载 IDTR 作准备
	xor	eax, eax
	mov	ax, ds
	shl	eax, 4
	add	eax, LABEL_IDT		; eax <- idt 基地址
	mov	dword [IdtPtr + 2], eax	; [IdtPtr + 2] <- idt 基地址

	; 保存 IDTR
	sidt	[_SavedIDTR]

	; 保存中断屏蔽寄存器(IMREG)值
	in	al, 21h		;端口21h用于访问 8259A 的中断屏蔽寄存器
	mov	[_SavedIMREG], al

	; 加载 GDTR
	lgdt	[GdtPtr]

	; 关中断
	cli

	; 加载 IDTR
	lidt	[IdtPtr]

	; 打开地址线A20
	in	al, 92h
	or	al, 00000010b
	out	92h, al

	; 准备切换到保护模式
	mov	eax, cr0
	or	eax, 1	; 设置PE位为1
	mov	cr0, eax

	; 真正进入保护模式
	jmp	dword SelectorCode32:0	; 把代码段选择子 SelectorCode32 装入 cs, 并跳转到 Code32Selector:0  处
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

LABEL_REAL_ENTRY:		; 从保护模式跳回到实模式就到了这里
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, [_wSPValueInRealMode]

	lidt	[_SavedIDTR]	; 恢复 IDTR 的原值

	mov	al, [_SavedIMREG]	; ┓恢复中断屏蔽寄存器(IMREG)的原值
	out	21h, al				; ┛

	in	al, 92h			; ┓
	and	al, 11111101b	; ┣ 关闭 A20 地址线
	out	92h, al			; ┛

	sti			; 开中断

	mov	ax, 4c00h	; ┓
	int	21h			; ┛回到 DOS
; END of [SECTION .s16]###################################################################################################


; ########################################################################################################################
[SECTION .s32]; 32 位代码段. 由实模式跳入.
[BITS	32]
LABEL_SEG_CODE32:
	; 加载选择子初始化各段寄存器
	mov	ax, SelectorData
	mov	ds, ax			; 数据段选择子
	mov	es, ax
	mov	ax, SelectorVideo
	mov	gs, ax			; 视频段选择子
	mov	ax, SelectorStack
	mov	ss, ax			; 堆栈段选择子
	mov	esp, TopOfStack

	; 初始化时钟和中断芯片--------------
	call	Init8253A
	call	Init8259A
	; 清空屏幕便于之后显示
	call	ClearScreen
	; -------------------------------
	; 下面显示进入保护模式，打印内存信息
	push	szPMMessage
	call	DispStr
	add	esp, 4
	push	szMemChkTitle
	call	DispStr
	add	esp, 4
	call	DispMemSize		; 显示内存信息

; 启动分页机制======================================================================
SetupPaging:
	; 根据内存大小计算应初始化多少PDE以及多少页表
	xor	edx, edx
	mov	eax, [dwMemSize]
	mov	ebx, 400000h	; 400000h = 4M = 4096 * 1024, 一个页表对应的内存大小
	div	ebx
	mov	ecx, eax	; 此时 ecx 为页表的个数，也即 PDE 应该的个数
	test	edx, edx
	jz	.no_remainder
	inc	ecx		; 如果余数不为 0 就需增加一个页表
.no_remainder:
	mov	[PageTableNumber], ecx	; 暂存页表个数
	; 为简化处理, 所有线性地址对应相等的物理地址. 并且不考虑内存空洞
	; 初始化每个任务的页目录--------------------------------------------
	call	LABEL_INIT_PAGE_TABLE0
	call	LABEL_INIT_PAGE_TABLE1
	call	LABEL_INIT_PAGE_TABLE2
	call	LABEL_INIT_PAGE_TABLE3
	; 初始化时钟-------------------------------------------------------
	xor 	ecx, ecx 
.initTicks:
	; 优先级赋值为执行时间片
	mov eax, dword [TaskPriority + ecx*4]             
	mov dword [ExecutionTicks + ecx*4], eax
	inc ecx
	cmp ecx, 4
	jne .initTicks			; 循环初始化4个任务的优先级
	xor ecx, ecx  
	sti						; 打开中断
	; 开启分页机制
	mov	eax, PageDirBase0	; ┳ 加载 CR3
	mov	cr3, eax			; ┛
	mov	ax, SelectorTSS0	; ┳ 加载 TSS 到任务寄存器TR中
	ltr	ax					; ┛
	mov	eax, cr0			; ┓
	or	eax, 80000000h		; ┣ 打开分页,标志位PG设为1
	mov	cr0, eax			; ┃
	jmp	short .3			; ┛
.3:
	nop
	; 初始化完成，分页机制启动完毕
.ready:
	xor 	ecx, ecx
	mov		ah, 0Fh
.outputLoop:
	mov		al, [szOwnerMessage + ecx]
	mov 	[gs:((80 * 19 + ecx) * 2)], ax
	inc		ecx
	cmp		al, 0
	jnz		.outputLoop
	; 开始执行任务代码^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	SwitchTask 0
	call	SetRealmode8259A	; 恢复8259A以顺利返回实模式，未执行, 因为任务死循环跳不出
	jmp		SelectorCode16:0	; 到此停止，返回实模式，未执行
; 保护模式结束^-^-------------------------------------------------------------------

SetRealmode8259A:
	mov		ax, SelectorData
	mov		fs, ax

	mov		al, 017h
	out		020h, al	; 主8259, ICW1.
	call	io_delay

	mov		al, 008h	; IRQ0 对应中断向量 0x8
	out		021h, al	; 主8259, ICW2.
	call	io_delay

	mov		al, 001h
	out		021h, al	; 主8259, ICW4.
	call	io_delay

	mov		al, [fs:SavedIMREG]	; ┓恢复中断屏蔽寄存器 IMREG 的原值
	out		021h, al			; ┛
	call	io_delay

	ret
; END of SetRealmode8259A

; int handler
; 时钟中断处理=======================================================================================
_ClockHandler:
ClockHandler	equ	_ClockHandler - $$
	push	ds
	pushad
	mov		eax, SelectorData
	mov		ds, ax
	mov		al, 20h
	out		20h, al		;发送 EOI，结束中断并等待下一次中断

	; 判断ExecutionTick是否为0，如果不为0，则说明当前任务未执行完，无需进行任务切换
	mov     edx, dword [RunningTask]               
	mov     ecx, dword [ExecutionTicks+edx*4]         
	test    ecx, ecx                              
	jnz     .subTicks	; 无需进行任务切换，跳到subTicks，继续执行当前任务且时间片-1
	; 否则，当前任务已经执行完成。
	; 继续判断任务是否已经全部完成，是则重新赋值                              
	mov     ebx, edx
	mov     eax, dword [ExecutionTicks]                                                
	or      eax, dword [ExecutionTicks + 4]                      
	or      eax, dword [ExecutionTicks + 8]                      
	or      eax, dword [ExecutionTicks + 12]                      
	jz      .initTask	; 任务全部完成，重新赋值
	; 否则没有全部完成，进行任务切换
.goToNext:  ; 选择下一个任务
	xor     esi, esi
	xor     eax, eax                               
	xor		ecx, ecx                               
.getMaxLoop:  ;	获取Ticks最大的任务
	cmp     dword [ExecutionTicks+eax*4], ecx           
	jle     .notMax                                   
	mov     ecx, dword [TaskPriority+eax*4]        
	mov     ebx, eax                                
	mov     esi, 1                                  
.notMax:  
	add     eax, 1                                  
	cmp     eax, 4                                  
	jnz     .getMaxLoop    ; 循环获取Ticks最大的任务
	mov     eax, esi                                
	test    al, al                                  
	jz      .subTicks                                   
	mov     dword [RunningTask], ebx    ; RunningTask存储优先级最高的任务    
	mov     edx, ebx
	SwitchTasks 	; 切换到目标任务
.subTicks:  ; 继续执行当前任务，时间片-1
	sub     dword [ExecutionTicks+edx*4], 1            
	jmp	 .exit                       
.initTask:  ; 全部任务均已结束，重新赋值
	xor		ecx, ecx
.setLoop:
	mov     eax, dword [TaskPriority + ecx*4]             
	mov     dword [ExecutionTicks + ecx*4], eax
	inc   	ecx
	cmp    	ecx, 4
	jne     .setLoop
	xor 	ecx, ecx                   
	jmp     .goToNext  ; 赋值后按照优先级选择任务                   
.exit:
	popad
	pop		ds
	iretd
; 时钟中断处理结束=======================================================================================

; 其他中断处理代码
_UserIntHandler:
UserIntHandler	equ	_UserIntHandler - $$
	mov	ah, 0Ch				; 0000: 黑底    1100: 红字
	mov	al, 'I'
	mov	[gs:((80 * 0 + 70) * 2)], ax	; 屏幕第 0 行, 第 70 列。
	iretd

_SpuriousHandler:
SpuriousHandler	equ	_SpuriousHandler - $$
	mov	ah, 0Ch				; 0000: 黑底    1100: 红字
	mov	al, '!'
	mov	[gs:((80 * 0 + 75) * 2)], ax	; 屏幕第 0 行, 第 75 列。
	iretd
; end of int handle

; 初始化任务页
InitPageTable 0
InitPageTable 1
InitPageTable 2
InitPageTable 3

%include "lib.inc"
SegCode32Len	equ	$ - LABEL_SEG_CODE32	; 保护模式代码长度
; END of [SECTION .s32]###################################################################################################


; #########################################################################################################################
; 16 位代码段, 由 32 位代码段跳入, 跳出后到实模式 实际上未执行
[SECTION .s16code]
ALIGN	32
[BITS	16]
LABEL_SEG_CODE16:
	; 跳回实模式:
	mov	ax, SelectorNormal
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax
	mov	ss, ax

	mov	eax, cr0
	and	al, 11111110b	; 仅切换到实模式
	; and	eax, 7ffffffeh	; 切换到实模式并关闭分页
	mov	cr0, eax

LABEL_GO_BACK_TO_REAL:
	jmp		0:LABEL_REAL_ENTRY	; 段地址会在程序开始处被设置成正确的值

Code16Len	equ	$ - LABEL_SEG_CODE16
; END of [SECTION .s16code]###################################################################################################