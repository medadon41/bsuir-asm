.model small
.stack 256
.data
    strerr db "Wrong input$"
    strerr2 db "Wrong size$"

    n dw ?
    m dw ?
    arr dw 10000 dup(0)

    max db 20
    len db 0
    buff db 200 dup (0)

.code

error:
    mov ah, 09h
    mov dx, offset strerr
    int 21h
    jmp exit 


error2:
    mov ah, 09h
    mov dx, offset strerr2
    int 21h
    jmp exit 


checkstr:
    push ax
    push si

    mov si, ax

checksym:
    mov al, [si]
    inc si

    cmp al, 0dh
    je endcheck

    cmp al, 20h
    jl error

    cmp al, 39h
    ja error

    sub al, 21h;
    cmp ax, 03h
    jl error

    jmp checksym
endcheck:
    ; popping registers
    pop si
    pop ax
    ret


space_count:
    ; ax - string
    push si
    mov si, ax
    mov ax, 0
check:
    mov al, [si]
    inc si
    cmp al, 0dh
    je end_sc
    cmp al, 20h
    jne notspace
    inc ah
notspace:
    jmp check
end_sc:
    mov al, ah
    mov ah, 0
    pop si
    ret


fill_array:
    ; ax - n
    ; bx - m
    ; dx - arr
    mov cx, ax
    push ax
array_loop:
    call get_string
    push ax
    call space_count
    inc ax
    cmp bx, ax
    jne error
    pop ax
    push bx
    mov bx, dx
    call str_to_arr
    pop bx
    add dx, bx
    add dx, bx
    loop array_loop
    pop ax
    ret


str_to_arr:

    push ax
    push bx
    push si
    mov si, ax
cycle:
    call atoi
    mov [bx], ax
checkspaces:
    mov al, [si]
    inc si
    cmp al, 0dh
    je end_sta
    cmp al, 20h
    jne checkspaces
    mov ax, si
    inc si
    add bx, 2
    jmp cycle
end_sta:
    pop si
    pop bx
    pop ax
    ret


atoi:
    ; pushing registers
    push bx
    push si
    push dx
    
    mov bx, 0
    mov si, ax
    mov ah, 0
gpos:
    mov al, [si]
    inc si
    cmp al, 20h
    je endn
    cmp al, 0dh
    je endn
    sub al, 48
    push ax
    mov ax, 10
    mul bx
    mov bx, ax
    pop ax
    add bx, ax
    jmp gpos
endn:
    mov ax, bx
    ; popping registers
    pop dx
    pop si
    pop bx
    ret


get_string:
    push dx
    mov ah, 0Ah
    mov dx, offset max
    int 21h

    mov dl, 10
    mov ah, 02h
    int 21h
    mov dl, 13
    mov ah, 02h
    int 21h

    mov ax, offset buff
    call checkstr
    pop dx
    ret

mffn:
    ; ax - n
    ; bx - m
    ; dx - arr

    mov cx, ax
    push dx
    mul bx
    pop dx
    mov bp, ax
push_loop1:
    mov si, cx
    mov cx, bx
push_loop2:


    mov ax, si
    add ax, cx
    push ax; 

    mov ax, bx
    push dx
    mul si
    pop dx
    sub ax, bx
    add ax, cx
    sub ax, 1
    shl ax, 1
    mov di, dx
    add di, ax

    mov ax, [di]
    push ax; 

    loop push_loop2
    mov cx, si

    loop push_loop1

    mov cx, bp
    mov bp, 0
    mov bx, 255
search_loop:
    pop ax
    pop di
    cmp ax, bx
    jnl old_min
new_min:
    mov dx, di
    mov bx, ax
old_min:
    loop search_loop

    mov ax, dx
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
    add ax, 1
ppos:
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


main:
    mov ax, @data
    mov ds, ax

    mov ax, 1
    mov bx, 2
    mov dx, offset n
    call fill_array

    mov ax, n
    mov bx, m
    mov dx, offset arr
    call fill_array


    mov ax, n
    mov bx, m
    mov dx, offset arr
    call mffn

    call printnum

exit:
    mov ah, 4Ch
    mov al, 0
    int 21h
end main