
global move_cursor

section .text

move_cursor:   ; pushfq
                push eax
                push ebx
                push ecx
                push edx
 
                ;unsigned short position = (row*80) + col;
                ;AX will contain 'position'
                mov ax,bx
                and ax,0xff             ;set AX to 'row'
                mov cl,80   
                mul cl                  ;row*80
 
                mov cx,bx               
                shr cx,8                ;set CX to 'col'
                add ax,cx               ;+ col
                mov cx,ax               ;store 'position' in CX
 
                ;cursor LOW port to vga INDEX register
                mov al,0xf             
                mov dx,0x3d4             ;VGA port 3D4h
                out dx,al             
 
                mov ax,cx               ;restore 'postion' back to AX  
                mov dx,0x3d5             ;VGA port 3D5h
                out dx,al               ;send to VGA hardware
 
                ;cursor HIGH port to vga INDEX register
                mov al,0xe
                mov dx,0x3d4             ;VGA port 3D4h
                out dx,al
 
                mov ax,cx               ;restore 'position' back to AX
                shr ax,8                ;get high byte in 'position'
                mov dx,0x3d5             ;VGA port 3D5h
                out dx,al               ;send to VGA hardware
 
                pop edx
                pop ecx
                pop ebx
                pop eax
                ;popfq
                ret
