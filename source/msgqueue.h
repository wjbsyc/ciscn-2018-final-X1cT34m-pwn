typedef struct corr
{
	char id[48];
	char result[128];
} Corr;

class Queue
{
public:
	Queue(int s);
	int push(Corr x);
	int pop();
	Corr * getTop();
	~Queue();
private:
	int size;
	int top, rear;
	Corr * Elements;
};

class QueueItem
{
public:
	QueueItem * next;
	QueueItem(int random_fd);
	char * getId();
	Queue * getQueue();
	~QueueItem();
private:
	char id[48];
	Queue * queue;
};

void str2hex(char * dest, unsigned char * src, int length);
