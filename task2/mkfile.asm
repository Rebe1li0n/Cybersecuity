Section .text
	global _start
_start:
	mov eax,0xffffffff
	xor eax,0xffffff9b
	add esp, eax
	xor eax, eax
	push eax
	push 0x68732f2f
	push 0x6e69622f
	mov ebx,esp	
	push eax
	mov ecx, 0xffffffff
	xor ecx, 0xffff9cd2
	push ecx
	mov ecx,esp
	xor edx,edx
	push edx
	push 0x7478742e
	push 0x666b673e
	push 0x22383130
	push 0x30393331
	push 0x30333731
	push 0x30322220
	push 0x6f686365
	mov edx,esp
	
	push eax
	push edx
	push ecx
	push ebx
	mov ecx,esp
	xor edx,edx
	mov al,0xb
	int 0x80
