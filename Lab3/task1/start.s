section .data
    line    db 10, 0
    data_pointer    dd 0
    infile      dd 0
    outfile     dd 1
    
section .bss
    cur         resb 256 
section .text
global _start
global system_call
extern strlen

_start:
    pop     dword ecx            ; ecx = argc
    mov     esi, esp            ; esi = argv
;; lea eax, [esi+4*ecx+4] ; eax = envp = (4*ecx)+esi+4
    mov     eax, ecx            ; put the number of arguments into eax
    shl     eax, 2              ; compute the size of argv in bytes
    add     eax, esi            ; add the size to the address of argv 
    add     eax, 4              ; skip NULL at the end of argv
    push    dword eax           ; char *envp[]
    push    dword esi           ; char* argv[]
    push    dword ecx           ; int argc

    call    main                ; int main(int argc, char *argv[], char *envp[])

    mov     ebx, eax
    mov     eax, 1
    int     0x80
    nop

system_call:
    push    ebp                 ; Save caller state
    mov     ebp, esp
    sub     esp, 4              ; Leave space for local var on stack
    pushad                      ; Save some more caller state
    
    mov     eax, [ebp+8]        ; Copy function args to registers: leftmost...
    mov     ebx, [ebp+12]       ; Next argument...
    mov     ecx, [ebp+16]       ; Next argument...
    mov     edx, [ebp+20]       ; Next argument...
    int     0x80                ; Transfer control to operating system
    mov     [ebp-4], eax        ; Save returned value...
    popad                       ; Restore caller state (registers)
    mov     eax, [ebp-4]        ; place returned value where caller can see it
    add     esp, 4              ; Restore caller state
    pop     ebp                 ; Restore caller state
    ret                         ; Back to caller

main:
    push    ebp
    mov     ebp, esp
    push    ebx
    mov     ecx, [ebp+8]        
    mov     edx, [ebp+12]       
    mov     ebx, 0              

next:
    mov     eax, dword [edx+ebx*4]
    pushad
    mov     ecx, eax
    pushad
    cmp     byte [ecx], '-'
    jne     debugging
    inc     ecx
    cmp     byte [ecx], 'i'
    je      handle_input
    cmp     byte [ecx], 'o'
    je      handle_output
    jmp     debugging

handle_input:
    inc     ecx
    mov     eax, 5          
    mov     ebx, ecx        
    mov     ecx, 2
    mov     edx, 0777        
    int     0x80
    mov     [infile], eax
    jmp     debugging

handle_output:
    inc     ecx
    mov     eax, 5          
    mov     ebx, ecx        
    mov     ecx, 0x41          
    mov     edx, 0777       
    int     0x80
    mov     [outfile], eax
    jmp     debugging
    
debugging:
    popad        
    mov     ebx, 1
    push    ecx
    call    strlen
    add     esp, 4
    mov     edx, eax
    mov     eax, 4
    int     0x80
    mov     ecx, line       
    mov     ebx, 2
    mov     eax, 4
    int     0x80
    popad
        
    inc     ebx
    cmp     ebx, ecx
    jnz     next
    add     esp, 4
    mov     [data_pointer], eax
    pop     ebx
    mov     esp, ebp
    pop     ebp


encode:

    mov     eax, 3              
    mov     ebx, [infile]
    mov     ecx, cur      
    mov     edx, 1
    int     0x80
    cmp     eax, 0           
    je      end                 


    cmp     byte [cur], 'a'
    jae     convert_to_next 
    cmp     byte [cur], 'A'
    jb      print_char          
    cmp     byte [cur], 'Z'
    jbe     convert_to_next   

print_char:
    mov     eax, 4              
    mov     ebx, [outfile]      
    mov     ecx, cur            
    mov     edx, 1              
    int     0x80                
    jmp     encode              

convert_to_next:
    inc     byte [cur]          
    jmp     print_char          

end:
    mov     eax, 1              
    xor     ebx, ebx            
    int     0x80                


