#include "rconn.h"

#include "post.h"
#include <math.h>
#include <system.h>

void ReceiveAck(int conn);

static const int MaxRMailSize = MaxMailSize - sizeof(RHeader);

RConn::RConn(PostOffice *post, int to_addr, int mailboxId)
    : mPost(post), addr(to_addr), mailbox(mailboxId), sendLock("conn send lock"), recvLock("conn recv lock")
{
	seqId = 1;
	friendSeqId = 1;
	closed = false;

	t = new Thread("ack receiver");

	t->Fork(ReceiveThread, (int)this);
}

RConn::~RConn()
{
}

void RConn::close()
{
	closed = true;
}

int RConn::send(int size, const char *data)
{
	sendLock.Acquire();

	for (int i = 0; i < size; i += MaxRMailSize) {
		int r = sendOne(std::min(MaxRMailSize, size - i), data + i);

		if (r != 0)
			return r;
	}

	sendLock.Release();

	return 0;
}

int RConn::sendOne(int size, const char *data)
{
	ASSERT(size <= MaxRMailSize);

	ROutMessage *mess = new ROutMessage;
	mess->id = seqId;
	mess->parent = this;
	mess->data = std::vector<char>(size, 0);
	memcpy(mess->data.data(), data, size);

	mess->ackReceived = false;
	mess->ackCond = new Semaphore("ack condvar", 0);

	mOutMessages[seqId] = mess;
	SendData(seqId, mess->data);

	seqId = (seqId % INT32_MAX) + 1;

	interrupt->Schedule(ProcAckSem, (int)mess->ackCond, REEMISSION_DELAY * 4800, NetworkSendInt);

	for (int i = 0; i < MAX_REEMISSIONS; i++) {
		mess->ackCond->Wait();

		if (mess->ackReceived) {
			mess->parent->mOutMessages.erase(mess->id);
			// delete mess->ackCond; // Possible use after free in interrupt handler
			delete mess;
			return 0;
		} else {
			DEBUG('n', "Trying to send message\n");
			mess->parent->SendData(mess->id, mess->data);
			interrupt->Schedule(ProcAckSem, (int)mess->ackCond, REEMISSION_DELAY * 4800, NetworkSendInt);
		}
	}

	if (!mess->ackReceived) {
		DEBUG('n', "Failed sending message after %d tries\n", MAX_REEMISSIONS);
	}

	return -1;
}

int RConn::recv(int size, char *data)
{
	recvLock.Acquire();

	for (int i = 0; i < size; i += MaxRMailSize) {
		int r = recvOne(std::min(MaxRMailSize, size - i), data + i);

		if (r != 0)
			return r;
	}

	recvLock.Release();

	return 0;
}

int RConn::recvOne(int size, char *data)
{
	ASSERT(size <= MaxRMailSize);

	while (!mInMessages.count(friendSeqId)) {
		currentThread->Yield();
	}

	RInMessage *in = mInMessages[friendSeqId];
	mInMessages.erase(friendSeqId);

	memcpy(data, in->data.data(), std::min(in->data.size(), (size_t)size));

	delete in;
	friendSeqId = (friendSeqId % INT32_MAX) + 1;

	return 0;
}

void RConn::ProcAckSem(int sem)
{
	Semaphore *sema = (Semaphore *)sem;
	sema->Post();
}

void RConn::SendAck(SeqId id)
{
	PacketHeader pktHdr;
	MailHeader mailHdr;

	pktHdr.to = addr;
	mailHdr.from = mailbox;
	mailHdr.to = mailbox;
	mailHdr.length = sizeof(RHeader);

	id = -id;

	mPost->Send(pktHdr, mailHdr, (const char *)&id);
}

void RConn::SendData(SeqId id, const std::vector<char> &data)
{
	ASSERT(data.size() <= MaxRMailSize);

	PacketHeader pktHdr;
	MailHeader mailHdr;

	pktHdr.to = addr;
	mailHdr.from = mailbox;
	mailHdr.to = mailbox;
	mailHdr.length = sizeof(RHeader) + data.size();

	char withHeader[MaxMailSize];
	memcpy(withHeader, &id, sizeof(SeqId));
	memcpy(withHeader + sizeof(SeqId), data.data(), data.size());

	mPost->Send(pktHdr, mailHdr, withHeader);
}

void RConn::ReceiveThread(int con)
{
	RConn *conn = (RConn *)con;
	PostOffice *post = conn->mPost;

	char data[MaxMailSize];

	while (!conn->closed) {
		PacketHeader pktH;
		MailHeader mailH;
		post->Receive(conn->mailbox, &pktH, &mailH, data);

		if (mailH.length >= sizeof(RHeader)) {
			RHeader *hdr = (RHeader *)data;

			if (hdr->id < 0) // Ack
			{
				DEBUG('n', "RECEIVED ACK\n");

				SeqId realSeqId = -hdr->id;
				if (conn->mOutMessages.count(realSeqId)) {
					conn->mOutMessages[realSeqId]->ackReceived = true;
					conn->mOutMessages[realSeqId]->ackCond->Post();
				}
			} else // Message
			{
				DEBUG('n', "RECEIVED MESSAGE\n");
				conn->SendAck(hdr->id);
				DEBUG('n', "ACK sent %d\n", hdr->id);

				if (hdr->id >= conn->friendSeqId && conn->mInMessages.count(hdr->id) == 0) {
					RInMessage *in = new RInMessage;
					in->parent = conn;
					in->id = hdr->id;
					in->data = std::vector<char>(mailH.length - sizeof(RHeader), 0);
					memcpy(in->data.data(), data + sizeof(RHeader), mailH.length - sizeof(RHeader));
					conn->mInMessages[hdr->id] = in;
				}
			}
		} else {
			DEBUG('n', "Malformed reliable packet of size %d\n", mailH.length);
		}
	}

	currentThread->Finish();
}