extern LoadGDT

%macro x86_EnterRealMode 0
	[bits 32]
	jmp word 18h:.pmode16		; jump to 16-bit protected mode segment

.pmode16:
	[bits 16]
	; 2 - disable protected mode bit in cr0
	mov eax, cr0
	and al, ~1
	mov cr0, eax

	; 3 - jump to real mode
	jmp word 00h:.rmode

.rmode:
	; 4 -setup segments
	mov ax, 0
	mov ds, ax
	mov ss, ax

	; 5 - enable interrupts
	sti

%endmacro


%macro x86_EnterProtectedMode 0
	cli
    call LoadGDT

	; 4 - setr protected enable flag in CR0
	mov eax, cr0
	or al, 1
	mov cr0, eax

	; 5 - far jump into protected mode
	jmp dword 08h:.pmode

.pmode:
    ; we are now in protected mode
	[bits 32]

	; 6 - setup segment registers
	mov ax, 0x10
	mov ds, ax
	mov ss, ax

%endmacro 

;	Convert linear adress to segment:offset adress
;	Args:
;		1 - linear address
;    	2 - (out) target segment (e.g es)	
;		3 - target 32 bit register to use (e.g eax)
; 		4 - target lower 16-bit half of #3 (e.g ax)

%macro	LinearToSegOffset 4

	mov %3, %1		; linear address to eax
	shr %3, 4
	mov %2, %4
	mov %3, %1		; linear address to eax
	and %3, 0xf

;
; 0x1000	0
;
; 0x10000 * 16 + 0

%endmacro

global x86_outb
x86_outb:
	[bits 32]
	mov dx, [esp + 4]
	mov al, [esp + 8]
	out dx, al
	ret

global x86_inb
x86_inb:
	[bits 32]
	mov dx, [esp + 4]
	xor eax, eax
	in al, dx
	ret

;
;
; bool _cdecl x86_Disk_GetDriveParams(uint8_t drive,
;                                     uint8_t* driveTypeOut,
;                                     uint16_t* cylindersOut,
;                                     uint16_t* sectorsOut,
;                                     uint16_t* headsOut);
;
global x86_Disk_GetDriveParams
x86_Disk_GetDriveParams:
	
	[bits 32]
    
	; make new call frame
    push ebp             ; save old call frame
    mov ebp, esp          ; initialize new call frame

	x86_EnterRealMode
	[bits 16]

    ; save regs
    push es
    push bx
    push esi
    push di

    ; call int13h
    mov dl, [bp + 8]    ; dl - disk drive
    mov ah, 08h
    mov di, 0           ; es:di - 0000:0000
    mov es, di
    stc
    int 13h

    ; out params
    mov eax, 1
    sbb eax, 0

	; drive type from bl
	LinearToSegOffset [bp + 12], es, esi, si
    mov es:[si], bl

    mov bl, ch          ; cylinders - lower bits in ch
    mov bh, cl          ; cylinders - upper bits in cl (6-7)
    shr bh, 6
	inc bx

	LinearToSegOffset [bp + 16], es, esi, si
    mov es:[si], bx

	; sectors
    xor ch, ch          ; sectors - lower 5 bits in cl
    and cl, 3Fh

	LinearToSegOffset [bp + 20], es, esi, si
    mov es:[si], cx

	; heads
    mov cl, dh          ; heads - dh
	inc cx
	LinearToSegOffset [bp + 24], es, esi, si
    mov es:[si], cx

    ; restore regs
    pop di
    pop esi
    pop bx
    pop es

    ; return
	push eax

	x86_EnterProtectedMode
	[bits 32]

	pop eax

    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret

;
; bool _cdecl x86_Disk_Reset(uint8_t drive);
;
global x86_Disk_Reset
x86_Disk_Reset:
	[bits 32]

    ; make new call frame
    push ebp             ; save old call frame
    mov ebp, esp          ; initialize new call frame

	x86_EnterRealMode
	[bits 16]

    mov ah, 0
    mov dl, [bp + 8]    ; dl - drive
    stc
    int 13h

    mov eax, 1
    sbb eax, 0           ; 1 on success, 0 on fail   

	push eax

	x86_EnterProtectedMode

	pop eax

    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret

;
; bool _cdecl x86_Disk_Read(uint8_t drive,
;                           uint16_t cylinder,
;                           uint16_t sector,
;                           uint16_t head,
;                           uint8_t count,
;                           void far * dataOut);
;

global x86_Disk_Read
x86_Disk_Read:

    ; make new call frame
    push ebp             ; save old call frame
    mov ebp, esp          ; initialize new call frame

	x86_EnterRealMode

    ; save modified regs
    push ebx
    push es

    ; setup args
    mov dl, [bp + 8]    ; dl - drive

    mov ch, [bp + 12]    ; ch - cylinder (lower 8 bits)
    mov cl, [bp + 13]    ; cl - cylinder to bits 6-7
    shl cl, 6
    
    mov al, [bp + 16]    ; cl - sector to bits 0-5
    and al, 3Fh
    or cl, al

    mov dh, [bp + 20]   ; dh - head

    mov al, [bp + 24]   ; al - count

	LinearToSegOffset [bp + 28], es, ebx, bx

    ; call int13h
    mov ah, 02h
    stc
    int 13h

    ; set return value
    mov eax, 1
    sbb eax, 0           ; 1 on success, 0 on fail   

    ; restore regs
    pop es
    pop ebx

	push eax
	x86_EnterProtectedMode
	pop eax

    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret

;
;   int 	__attribute__((cdecl)) x86_Video_GetVbeInfo(void* infoOut);
;
global x86_Video_GetVbeInfo
x86_Video_GetVbeInfo:
    ; make new call frame
    push ebp             ; save old call frame
    mov ebp, esp          ; initialize new call frame

	x86_EnterRealMode

    push edi
    push es
    push ebp

    mov eax, 0x4f00
    LinearToSegOffset [bp + 8], es, edi, di
    int 10h

    pop ebp
    pop es
    pop edi

	push eax
	x86_EnterProtectedMode
	pop eax

    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret

;
;   int 	__attribute__((cdecl)) x86_Video_GetModeInfo(uint16_t mode, void* infoOut);
;   
global x86_Video_GetModeInfo
x86_Video_GetModeInfo:
    ; make new call frame
    push ebp             ; save old call frame
    mov ebp, esp         ; initialize new call frame

	x86_EnterRealMode

    push edi
    push es
    push ebp
    push ecx

    mov ax, 0x4f01
    mov cx, [bp + 8]
    LinearToSegOffset [bp + 12], es, edi, di
    int 10h

    pop ecx
    pop ebp
    pop es
    pop edi

	push eax
	x86_EnterProtectedMode
	pop eax

    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret

;
;   int 	__attribute__((cdecl)) x86_Video_SetMode(uint16_t mode);
;
global x86_Video_SetMode
x86_Video_SetMode:
    ; make new call frame
    push ebp             ; save old call frame
    mov ebp, esp          ; initialize new call frame

	x86_EnterRealMode

    push edi
    push es
    push ebp
    push ebx

    mov ax, 0
    mov es, ax
    mov edi, 0
    mov ax, 0x4f02
    mov bx, [bp + 8]
    int 10h

    pop ebx
    pop ebp
    pop es
    pop edi

	push eax
	x86_EnterProtectedMode
	pop eax

    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret