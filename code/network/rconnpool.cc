#include "rconnpool.h"

RConnPool::RConnPool(PostOffice *post) : mPost(post)
{
}

int RConnPool::connect(int addr, int mailbox)
{
	return -1;
}