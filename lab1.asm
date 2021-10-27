.model small
.stack 256
.8086
.data
a dw ?
b dw ?
c dw ?
d dw ?

.code
main:
    mov ax, @data
    mov ds, ax
    ; Read numbers
    call readnum
    mov a, bx
    call readnum
    mov b, bx
    call readnum
    mov c, bx
    call readnum
    mov d, bx
    ; check:  if ((b * с) != (d - a))
    mov ax, b
    mul c

    mov bx, d
    sub bx, a
    cmp ax, bx

    jne isTrue

    ; if is not true then check the second condition
    ; check:  if (b < c)
    mov ax, b
    cmp ax, c

    jb isTrue
    jnb notTrue

isTrue:
    ; print(3 * a + b * (c - d))
    mov ax, c
    sub ax, d
    mul b 
    mov bx, ax
    mov ax, 3
    mul a
    add ax, bx
    jmp print

notTrue:
    ; check:  if ((a - d) < (b + c))
    mov ax, a
    sub ax, d 
    mov bx, b
    add bx, c 
    cmp ax, bx

    jnb innerIsFalse

    ; check:  if (a < b)
    mov ax, a
    cmp ax, b
    jb innerIsTrue
    jnb innerIsFalse

innerIsFalse:
    ; print(2 * b - 5 * d + 3)
    mov ax, 2
    mul b
    mov bx, ax
    mov ax, 5
    mul d
    sub ax, bx 
    add ax, 3
    jmp print

innerIsTrue:
    ; print(a * a - b + c)
    mov ax, a
    mul a
    sub ax, b
    add ax, c
    jmp print

; print and read numbers
print:
	call printnum
exit:
    mov ah, 04Ch
    mov al, 0
    int 21h
readnum:
	mov bx, 0
    mov ah, 01h
    int 21h
    cmp al, 2dh
	je negative
	call analyze
	ret
negative:
	call rpos
	not bx 
	inc bx
	ret
rpos:
    mov ah, 01h
    int 21h
analyze:
    cmp al, 0dh
    je endl
    cmp al, 10
    je endl
    sub al, 48
    mov ah, 0
	push ax
	mov ax, 10
    mul bx
    mov bx, ax
	pop ax
    add bx, ax
    call rpos
endl:
	ret
printnum:
	cmp ax, 0
	jz pzero
	jnl ppos
	mov dl, '-'
	push ax
	mov ah, 02h
	int 21h
	pop ax
	not ax 
	inc ax
ppos:
;used: ax dx bx 
	cmp ax, 0
	jz zero
	mov dx, 0
	mov bx, 10
	div bx    
	add dl, 48
	push dx
	call ppos
	pop dx
	push ax
	mov ah, 02h
	int 21h
	pop ax
zero:
	ret
pzero:
	mov dl, 30h
	mov ah, 02h
	int 21h
	ret
end main