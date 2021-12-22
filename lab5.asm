.386

.model tiny

.stack 100h

data segment use16
    text_first           db 'Input number', 10, '$'
    text_second          db 10, 'Interruption handler changed. Input one more number', 10, '$'
    crlf                 db 0Ah,'$'
    buffer_seg           dw 0
    buffer_set_off       dw 0
    flag_active          dw 0
    global_param         db 5
    
data ends

code segment use16
    assume cs: code, ds: data

    RESIDENT:
        org 100h
        jmp INSTALL

        custom_input proc
            mov cx, 37h
            mov si, word ptr cs:[old_interupt]
            mov di, word ptr cs:[old_interupt + 2]
            push si di ds cx bx ax
            pushf                                                    ; push flag register values
            cmp ah, 02h                                              ; check if we have necessary function and if no then go to default one
            jne SKIP_MY_INTERRUPT

            mov cl, global_param
            mov cs:[param], cl

            cmp dl, 48                                               ; check that it is number
            jl SKIP_MY_INTERRUPT
            cmp dl, 57
            jg SKIP_MY_INTERRUPT
            add dl, cs:[param]                                       ; cycle move
            cmp dl, 58
            jle SKIP_MY_INTERRUPT                                    ; if (result > 58) then res -= 10, because it is cycle move
            sub dl, 10
            SKIP_MY_INTERRUPT:
                popf
                pop ax bx cx ds di si
                jmp cs:[old_interupt]                                ; jump to old handler of interruption
            ret
            old_interupt dd ?
            param db 0
    custom_input endp

    INSTALL:
    main proc
        INITIALIZE_DS:
            mov ax, data
            mov ds, ax

        CHECK_ARGS_COUNT:                                            ; taking digit from console arguments
            mov si, 80h
            mov al, es:[si]
            cmp al, 0
            je SET_DEFAULT_PARAM
   
            mov si, 82h
            mov al, es:[si]
            cmp al, 47
            jle SET_DEFAULT_PARAM
            cmp al, 58
            jge SET_DEFAULT_PARAM
            sub al, 48
            mov global_param, al
            jmp AFTER_CHECKING_CONSOLE_ARS
     
        SET_DEFAULT_PARAM:
            mov global_param, 5

        AFTER_CHECKING_CONSOLE_ARS:

        mov dx, offset text_first
        call cout_string

        mov ah, 01h
        int 21h

        ; mov dl, al
        ; call cout_char

        cmp cx, 37h
        je RETURN_HANDLER                                            ; if does not equal then install our own program
        push ds
        mov ax, code
        mov ds, ax

        mov ah, 35h                                                  ; ah = 35h - get int vector
        mov al, 21h                                                  ; Entry: al = int number
        int 21h                                                      ; Return: es:bx -> current int handler

        mov word ptr old_interupt, bx
        mov word ptr old_interupt + 2, es

        mov ah, 25h                                                  ; ah = 25h - set int vector
        mov al, 21h                                                  ; al = int number
        mov dx, offset custom_input                                  ; ds:dx -> new int handler
        int 21h

        pop ds
        jmp PROCES_PROGRAM

        RETURN_HANDLER:
            xor cx, cx
            push ds
            mov ax, di
            mov ds, ax

            mov ah, 25h                                              ; ah = 25h - set int vector
            mov al, 21h                                              ; al = int number
            mov dx, si                                               ; ds:dx -> new int handler
            int 21h

            pop ds

            mov dx, offset text_second
            call cout_string

            mov ah, 01h
            int 21h

            mov dx, offset crlf
            call cout_string

            mov dl, al
            call cout_char

            RETURN_FROM_MAIN:
                mov ah, 04ch
                int 21h

        PROCES_PROGRAM:  
            mov dx, offset text_second
            call cout_string

            xor dl, dl
            xor dx, dx

            mov ah, 01h
            int 21h

            mov dx, offset crlf
            call cout_string
            
            mov dl, al
            call cout_char

            mov ax, byte [flag_active]
            mov ax, word ptr[buffer_seg]
            mov ax, word ptr[buffer_set_off]

        CLOSE:
            mov ax, 3100h
            mov dx, (resident - install + 10Fh) / 16
            int 27h
    main endp

    cout_char proc
        mov ah, 02h
        int 21h
        ret
    cout_char endp
    
    cout_string proc 
        mov ah, 09h
        int 21h
        ret
    cout_string endp 
code ends
    end main