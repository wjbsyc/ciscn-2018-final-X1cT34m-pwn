#include "msgqueue.h"
#include <unistd.h>
#include <stdlib.h>

char hex_chars[] = "0123456789abcdef";

Queue::Queue(int s)
{
	this -> size = s;
	this -> Elements = (Corr *)malloc(sizeof(Corr) * s);
	this -> top = 0;
	this -> rear = 0;
	return;
}

Queue::~Queue()
{
	free(this -> Elements);
	return;
}

int Queue::push(Corr x)
{
	if ((this -> rear + 1) % (this -> size) == this -> top)
	{
		return -1;
	}
	this -> Elements[this -> rear] = x;
	this -> rear = (this -> rear + 1) % this -> size;
	return 0;
}

int Queue::pop()
{
	if (this -> top == this -> rear)
	{
		return -1;
	}
	this -> top = (this -> top + 1) % this -> size;
	return 0;
}

Corr * Queue::getTop()
{
	return (this -> Elements + this -> top);
}

QueueItem::QueueItem(int random_fd)
{
	unsigned char id_buf[16];
	read(random_fd, id_buf, 16);
	this -> next = NULL;
	this -> queue = new Queue(0x80);
	str2hex(this -> id, id_buf, 16);
	return;
}

QueueItem::~QueueItem()
{
	free(this -> queue);
	return;
}

char * QueueItem::getId()
{
	return this -> id;
}

Queue * QueueItem::getQueue()
{
	return this -> queue;
}

void str2hex(char * dest, unsigned char * src, int length)
{
	for (int i = 0; i < length; i++)
	{
		*(dest + i * 2) = hex_chars[src[i] / 16];
		*(dest + i * 2 + 1) = hex_chars[src[i] % 16];
	}
	return;
}