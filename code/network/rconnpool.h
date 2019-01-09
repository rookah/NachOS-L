#ifndef RCONNPOOL_H
#define RCONNPOOL_H

#include <unordered_map>

class PostOffice;
class RConn;

class RConnPool
{

  public:
	RConnPool(PostOffice *post);

	int connect(int addr, int mailbox); // Return -1 if mailbox not available
	void close(int connId);

	int send(int connId, int size, char *data);
	int recv(int connId, int size, char *data);

  private:
	PostOffice *mPost;
	std::unordered_map<int, RConn *> mConnList;
	unsigned int nextId;
};

#endif
