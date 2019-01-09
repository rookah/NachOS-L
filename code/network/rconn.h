#ifndef RELIABLEPOST_H
#define RELIABLEPOST_H

#include <unordered_map>
#include <vector>

typedef int SeqId;

#define MAX_REEMISSIONS 32
#define REEMISSION_DELAY 100

class PostOffice;
class Thread;
class RConn;
class Semaphore;

struct ROutMessage {
	RConn *parent;
	std::vector<char> data;
	bool ackReceived;
	SeqId id;
	Semaphore *ackCond;
};

struct RInMessage {
	RConn *parent;
	std::vector<char> data;
	SeqId id;
};

struct RHeader {
	SeqId id;
};

class RConn
{
  public:
	RConn(PostOffice *post, int to_addr, int mailboxId);
	void close();
	int send(int size, const char* data);
	int recv(int size, char *data);

  private:
	~RConn();
	static void ProcAckSem(int mess);
	static void ReceiveThread(int mailHdr);
	void SendAck(SeqId id);
	void SendData(SeqId id, const std::vector<char> &data);

	PostOffice *mPost;
	Thread* t;
	SeqId seqId; // Between 1 and INT32_MAX
	SeqId friendSeqId;
	int addr;
	int mailbox;
	bool closed;

	std::unordered_map<SeqId, ROutMessage *> mOutMessages;
	std::unordered_map<SeqId, RInMessage *> mInMessages;
};

#endif
