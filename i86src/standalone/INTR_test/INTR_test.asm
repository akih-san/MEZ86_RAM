	page 0
	CPU	8086

CSEG	equ	40h

MON_SEG		equ	0ff50h
REQ_CONIN	equ	1
REQ_CONOUT	equ	2
REQ_CONST	equ	3
REQ_STROUT	equ	4
DUMMY_PORT	equ	0f000h
TERMINATE	equ	0ffh

	SEGMENT	CODE

	org	0

test_start:
	cli
	mov	ax, cs
	mov	ds, ax		; ds = cs = CSEG
	xor	ax, ax
	mov	es, ax		; es = 0
	
;
; set int20h interrupt handler address

	mov	di, 20h*4
	mov	si, int20h_vec
	mov	cx, 2
	cld
	rep	movsw
	
; opening msg

	mov	si, st_msg
openmsg:
	mov	al ,[si]
	and	al, al
	jz	test_1
	call	conout
	inc	si
	jmp	openmsg
	
test_1:
	mov	bx, int_flg
	mov	word ptr [int_cnt], 0
	mov	byte ptr [bx], 0
	sti
loop:
	mov	al, [bx]
	and	al, al
	jz	loop

	mov	si, intr_msg
putmsg:
	mov	al ,[si]
	and	al, al
	jz	test_2
	call	conout
	inc	si
	jmp	putmsg
test_2:
	cli
	jmp	test_1

;
; int 20h handler
;
int20h:
	push	ds
	push	bx
	push	ax
	
	push	cs
	pop	ds
	mov	bx, int_cnt
	mov	ax, [int_cnt]
	inc	ax
	cmp	ax, 300
	jne	add_cnt

	mov	byte ptr [int_flg], 1
	xor	ax, ax
add_cnt:
	mov	[bx], ax

	pop	ax
	pop	bx
	pop	ds
	iret

;
; PIC I/F
;
conout:
	push	ds
	push	bx
	mov	bx, MON_SEG
	mov	ds, bx
	mov	byte ptr [UREQ_COM], REQ_CONOUT	; set CONOUT request
	mov	byte ptr [UNI_CHR], al		; set char

	push	dx
	mov	dx, DUMMY_PORT
	in	al, dx				; invoke PIC F/W
wait_:
	mov	al, [UREQ_COM]
	or	al, al
	jnz	wait_

	pop	dx
	mov	al, [UNI_CHR]			; get char or status
	and	al, al

	pop	bx
	pop	ds
	ret

int20h_vec:
	dw	int20h
	dw	CSEG

st_msg:		db	"INTR (INT20H) Interrupt test",13,10,0
intr_msg:	db	"Interrupt INT20H has occured 300 times.",13,10,0

	ALIGN	2
int_flg:	ds	1

	ALIGN	2
int_cnt:	ds	2


	SEGMENT	DATA
	org	0

; ------------------ unimon Command 
UREQ_COM	ds	1	; location 00	:unimon request command
; ------------------ unimon CONSOLE I/O
UNI_CHR		ds	1	; location 01	:charcter (CONIN/CONOUT) or counts(STROUT) or status
STR_OFF		ds	2	; location 02	:string offset
STR_SEG		ds	2	; location 04	:string segment
; ------------------ CBIOS Command
CREQ_COM	ds	1	; location 06	:CBIOS request command
; ------------------ CBIOS CONSOLE I/O
CBI_CHR		ds	1	; location 07	:charcter (CONIN/CONOUT) or counts(STROUT) or status
disk_drive	ds	1	; location 08	:
disk_track	ds	1	; location 09	:
disk_sector	ds	2	; location 0A	:
data_dmal	ds	2	; location 0C	:
data_dmah	ds	2	; location 0E	:


	END
