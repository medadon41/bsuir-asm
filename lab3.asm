.model small
.stack 100h
.data
    message_in_str      db "Please enter your string: $" 
    message_err         db "Bad input$" 
    message_res         db "Sorted string: $"
    crlf                db 0Dh,0Ah,'$'                 
    b_max_size          db 200                         
    b_length            db (?)                         
    buff                db 200 dup (?)

.code

output_str macro                                        
    push ax
    mov ah,09h
    int 21h
    pop ax
endm

input_string macro                                      
    push ax
    mov ah, 0ah
    int 21h
    pop ax
endm

output_char macro
    push ax
    mov ah,02h
    int 21h
    pop ax
endm

check_input proc  
    push bp    
    mov bp, sp
    push cx
    push dx
    push bx

    mov bx, [bp + 4]
    xor cx, cx   
    mov cl, [b_length]        

check:
    mov dl, byte ptr [bx] 

    cmp dl, 41h
    jb error

    cmp dl, 7ah
    ja error   

    cmp dl, 5ah
    jb label1

    cmp dl, 61h
    jb error

label1: 
    inc bx
loop check

jmp end_check

error:
    lea dx, message_err
    output_str
    jmp end_of_pr

end_check:
    pop bx
    pop dx
    pop cx
    mov sp, bp
    pop bp  
    ret 
endp check_input

print_string proc
    push bp              
    mov bp, sp
    push cx
    push dx

    mov bx, [bp + 4]
    xor cx, cx   
    mov cl, [b_length]        

print:
    mov dl, byte ptr [bx] 
    output_char
    inc bx
loop print

    pop dx
    pop cx
    mov sp,bp
    pop bp      
    ret 2
endp print_string


q_sort proc
    push bp                                             
    mov bp, sp   
    push ax                                             
    push bx
    push cx 
    push di   
    push dx   
    mov bx, [bp + 6]                                    
    mov di, [bp + 4]
    cmp bx, di                                          
    jae return                                             
    push bx   
    push di   
    call pick_pivot
    pop ax                                              
    push bx                                            
    push di  
    push ax    
    call split
    pop dx                                              
    push bx           
    push dx 
    call q_sort                                         
    pop dx                                              
    push dx  
    push di   
    call q_sort                                         
return:
    pop dx
    pop di      
    pop cx   
    pop bx      
    pop ax 
    mov sp,bp
    pop bp      
    ret 4
endp q_sort

pick_pivot proc
    push bp                                             
    mov bp, sp 
    push bx                                             
    push di   
    push ax
    mov bx, [bp + 6]                                    
    mov di, [bp + 4]                
    add bx, di                                          
    shr bx, 1
    xor ax, ax
    mov al, byte ptr [bx]
    mov [bp + 6], ax                                    
    pop ax
    pop di   
    pop bx 
    mov sp, bp
    pop bp    
    ret 2                                               
endp pick_pivot

split proc    
    push bp  
    mov bp,sp   
    push ax 
    push bx  
    push cx 
    push dx 
    push di  
    mov bx, [bp + 8]                                    
    mov di, [bp + 6]                                    
    mov ax, [bp + 4]                                    
while_one_start:                                        
    cmp bx, di                                         
    ja while_one_end
    while_two_start:
        mov dh, byte ptr [bx]
        mov dl, al
        cmp dh, dl                                      
        jae while_two_end
        inc bx
        jmp while_two_start
    while_two_end:

    while_three_start:
        mov dh, byte ptr [di]                   
        cmp dh, al                                      
        jbe while_three_end
        dec di
        jmp while_three_start
    while_three_end:

    cmp bx, di      
    ja while_one_end
    push bx            
    push di    
    call swap  
    inc bx           
    dec di     
    jmp while_one_start
while_one_end:
    mov [bp + 6], di
    mov [bp + 8], bx       
    pop di     
    pop dx    
    pop cx   
    pop bx   
    pop ax 
    mov sp, bp
    pop bp   
    ret 2 
endp split

swap proc    
    push bp      
    mov bp, sp     

    push ax   
    push bx 
    push di 

    mov bx, [bp + 6]                                       
    mov di, [bp + 4]                                       

    mov al, [byte ptr bx]                                  
    xchg byte ptr [di], al                                 
    mov byte ptr [bx], al                                  

    pop di
    pop bx  
    pop ax 

    mov sp, bp
    pop bp
    ret 4
endp swap

start:
    mov ax,@data
    mov ds,ax

    lea dx, message_in_str
    output_str

    lea dx, b_max_size
    input_string

    lea dx, crlf
    output_str

    lea bx, byte ptr[buff]                                
    xor ax, ax
    mov al, byte ptr [b_length]  
    push bx                                               
    add bx, ax
    dec bx 
    mov di, bx
    pop bx

    push bx
    call check_input

    push bx                                               
    push di
    call q_sort

    lea dx, message_res
    output_str

    push bx
    call print_string

end_of_pr:
    MOV ax, 4C00h
    INT 21h
end start