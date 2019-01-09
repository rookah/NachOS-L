#include "rconnpool.h"

#include "rconn.h"

RConnPool::RConnPool(PostOffice *post) : mPost(post)
{
	nextId = 0;
}

int RConnPool::connect(int addr, int mailbox)
{
	mConnList[nextId] = new RConn(mPost, addr, mailbox);
	return nextId++;
}

int RConnPool::send(int connId, int size, char *data)
{
	if (mConnList.count(connId)) {
		return mConnList[connId]->send(size, data);
	}

	return -1;
}

int RConnPool::recv(int connId, int size, char *data)
{
	if (mConnList.count(connId)) {
		return mConnList[connId]->recv(size, data);
	}

	return -1;
}

void RConnPool::close(int connId)
{
	if (mConnList.count(connId)) {
		mConnList[connId]->close();
		mConnList.erase(connId);
	}
}