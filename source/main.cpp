#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>

#include "msgqueue.h"
#include "expr.h"

#define MAGIC_SEND 				"RPCM"
#define MAGIC_RECV 				"RPCN"

#define SEND_TYPE_CONNECT 		0
#define SEND_TYPE_DECLARE 		1
#define SEND_TYPE_RETRIEVE 		2
#define SEND_TYPE_CALL 			3
#define SEND_TYPE_CLOSE			4

#define RETN_TYPE_DONE			0xbeef
#define RETN_TYPE_ERROR			0xbeef + 1
#define RETN_TYPE_UNAVAILABLE	0xbeef + 2
#define RETN_TYPE_RESULT		0xbeef + 3

#define KEY_FIELD				fields[0]
#define CORR_ID_FIELD			fields[1]
#define REPLY_TO_FIELD			fields[0]
#define EXPR_FIELD				fields[2]
#define RESULT_FIELD			fields[0]

typedef struct field
{
	int length;
	unsigned char buf[128];
} Field;

typedef struct RPCMessage
{
	char magic[4];
	int length;
	int type;
	Field * fields[3];
} Message;

typedef struct params
{
	Message * msg;
	QueueItem * q;
} Params;

void init();
void return_package(int type, char * result = NULL, int result_length = 0);
char * declare_queue();
char * handle_retrieve(Message* msg);
int handle_call(Message * msg);
void* do_call(void * args);
void xread(int fd, void * buf, int size);

int connect_flag = 0;
int fd;
QueueItem * queue_list = NULL;

int main()
{
	init();
	Message * msg = (Message *)malloc(sizeof(Message));
	while(1)
	{
		xread(0, msg, 12);
		msg -> type = ntohl(msg -> type);
		if (memcmp(msg -> magic, MAGIC_SEND, 4))
		{
			return_package(RETN_TYPE_ERROR);
			continue;
		}
		if (connect_flag == 0)
		{
			if(msg -> type == SEND_TYPE_CONNECT)
			{
				connect_flag = 1;
				return_package(RETN_TYPE_DONE);
				continue;
			}
			else
			{
				return_package(RETN_TYPE_ERROR);
				continue;
			}
		}
		switch(msg -> type)
		{
			case SEND_TYPE_CONNECT:
			{
				return_package(RETN_TYPE_ERROR);
				break;
			}
			case SEND_TYPE_DECLARE:
			{
				char * queue_id = declare_queue();
				return_package(RETN_TYPE_RESULT, queue_id, 32);
				break;
			}
			case SEND_TYPE_RETRIEVE:
			{
				char * result = handle_retrieve(msg);
				if (result == 0)
				{
					return_package(RETN_TYPE_UNAVAILABLE);
				}
				else
				{
					return_package(RETN_TYPE_RESULT, result, strlen(result));
				}
				break;
			}
			case SEND_TYPE_CALL:
			{
				int success_flag = handle_call(msg);
				if (success_flag)
				{
					return_package(RETN_TYPE_DONE);
				}
				else
				{
					return_package(RETN_TYPE_ERROR);
				}
				break;
			}
			case SEND_TYPE_CLOSE:
			{
				close(fd);
				free(msg);
				return 0;
			}
			default:
			{
				return_package(RETN_TYPE_ERROR);
				break;
			}
		}
	}
	return 0;
}

void init()
{
	setbuf(stdin, 0);
	setbuf(stdout, 0);
	setbuf(stderr, 0);
	fd = open("/dev/urandom", O_RDONLY);
	if (fd == -1)
	{
		puts("Error!");
		exit(-1);
	}
	return;
}

void return_package(int type, char * result, int result_length)
{
	Message* msg = (Message *)malloc(sizeof(Message));
	strcpy(msg -> magic, MAGIC_RECV);
	char * ret_msg = (char *)malloc(0x200);
	int package_length = 12;
	msg -> length = htonl(12);
	msg -> type = htonl(type);
	if (type == RETN_TYPE_RESULT)
	{
		package_length += result_length + 4;
		msg -> length = htonl(package_length);
		msg -> RESULT_FIELD = (Field *)malloc(sizeof(Field));
		msg -> RESULT_FIELD -> length = htonl(result_length);
		memcpy(msg -> RESULT_FIELD -> buf, result, result_length);
	}
	//write(1, msg, 12);
	memcpy(ret_msg, msg, 12);
	if (type == RETN_TYPE_RESULT)
	{
		//write(1, msg -> RESULT_FIELD, result_length + 4);
		memcpy(ret_msg + 12, msg -> RESULT_FIELD, result_length + 4);
		free(msg -> RESULT_FIELD);
	}
	write(1, ret_msg, package_length);
	free(msg);
	free(ret_msg);
	return;
}

char * declare_queue()
{
	QueueItem* p;
	if (queue_list == NULL)
	{
		queue_list = new QueueItem(fd);
		return queue_list -> getId();
	}
	else
	{
		for(p = queue_list; p -> next != NULL; p = p -> next);
		p -> next = new QueueItem(fd);
	}
	return p -> next -> getId();
}

char * handle_retrieve(Message * msg)
{
	msg -> KEY_FIELD = (Field *)malloc(sizeof(Field));
	xread(0, &(msg -> KEY_FIELD -> length), 4);
	msg -> KEY_FIELD -> length = ntohl(msg -> KEY_FIELD -> length);
	xread(0, msg -> KEY_FIELD -> buf, msg -> KEY_FIELD -> length);
	msg -> CORR_ID_FIELD = (Field *)malloc(sizeof(Field));
	xread(0, &(msg -> CORR_ID_FIELD -> length), 4);
	msg -> CORR_ID_FIELD -> length = ntohl(msg -> CORR_ID_FIELD -> length);
	xread(0, msg -> CORR_ID_FIELD -> buf, msg -> CORR_ID_FIELD -> length);
	QueueItem * p = queue_list;
	while((p != NULL) && (memcmp(p -> getId(), msg -> KEY_FIELD -> buf, msg -> KEY_FIELD -> length)))
	{
		p = p -> next;
	}
	if (p == NULL)
	{
		free(msg -> KEY_FIELD);
		free(msg -> CORR_ID_FIELD);
		return NULL;
	}
	Corr * c = p -> getQueue() -> getTop();
	if(memcmp(c -> id, msg -> CORR_ID_FIELD -> buf, msg -> CORR_ID_FIELD -> length))
	{
		free(msg -> KEY_FIELD);
		free(msg -> CORR_ID_FIELD);
		return NULL;
	}
	char * res = c -> result;
	p -> getQueue() -> pop();
	free(msg -> KEY_FIELD);
	free(msg -> CORR_ID_FIELD);
	return res;
}

int handle_call(Message * msg)
{
	pthread_t t;
	msg -> REPLY_TO_FIELD = (Field *)malloc(sizeof(Field));
	xread(0, &(msg -> REPLY_TO_FIELD -> length), 4);
	msg -> REPLY_TO_FIELD -> length = ntohl(msg -> REPLY_TO_FIELD -> length);
	xread(0, msg -> REPLY_TO_FIELD -> buf, msg -> REPLY_TO_FIELD -> length);
	msg -> CORR_ID_FIELD = (Field *)malloc(sizeof(Field));
	xread(0, &(msg -> CORR_ID_FIELD -> length), 4);
	msg -> CORR_ID_FIELD -> length = ntohl(msg -> CORR_ID_FIELD -> length);
	xread(0, msg -> CORR_ID_FIELD -> buf, msg -> CORR_ID_FIELD -> length);
	msg -> EXPR_FIELD = (Field *)malloc(sizeof(Field));
	xread(0, &(msg -> EXPR_FIELD -> length), 4);
	msg -> EXPR_FIELD -> length = ntohl(msg -> EXPR_FIELD -> length);
	xread(0, msg -> EXPR_FIELD -> buf, msg -> EXPR_FIELD -> length); 
	QueueItem * p = queue_list;
	while((p != NULL) && (memcmp(p -> getId(), msg -> KEY_FIELD -> buf, msg -> KEY_FIELD -> length)))
	{
		p = p -> next;
	}
	if (p == NULL)
	{
		free(msg -> REPLY_TO_FIELD);
		free(msg -> CORR_ID_FIELD);
		free(msg -> EXPR_FIELD);
		return 0;
	}
	Params ps = {msg, p};
	//pthread_create(&t, NULL, do_call, (void *)&ps);
	//sleep(1);
	//pthread_join(t, NULL);
	do_call(&ps);
	free(msg -> REPLY_TO_FIELD);
	free(msg -> CORR_ID_FIELD);
	free(msg -> EXPR_FIELD);
	return 1;
}

void* do_call(void * args)
{
	Params* ps = (Params *)args;
	Message * msg = ps -> msg;
	QueueItem * q = ps -> q;
	Corr * new_corr = (Corr *)malloc(sizeof(Corr));
	memcpy(new_corr -> id, msg -> CORR_ID_FIELD -> buf, msg -> CORR_ID_FIELD -> length);
	*(msg -> EXPR_FIELD -> buf + msg -> EXPR_FIELD -> length) = 0;
	char * eval_result = eval_expr(msg -> EXPR_FIELD -> buf, msg -> EXPR_FIELD -> length + 1);
	memcpy(new_corr -> result, eval_result, strlen(eval_result) + 1);
	q -> getQueue() -> push(*new_corr);
	free(new_corr);
	return NULL;
}

void xread(int fd, void * buf, int size)
{
	unsigned char c;
	for (int i = 0; i < size; i++)
	{
		read(fd, &c, 1u);
		((unsigned char *)buf)[i] = c;
	}
	return;
}
