#include "rconn.h"

#include "post.h"
#include <system.h>

void ReceiveAck(int conn);

RConn::RConn(PostOffice *post, int to_addr, int mailboxId) : mPost(post), addr(to_addr), mailbox(mailboxId)
{
	seqId = 1;
	friendSeqId = 1;
	Thread *t = new Thread("ack receiver");

	t->Fork(ReceiveThread, (int)this);
}

RConn::~RConn()
{
}

void RConn::send(const std::vector<char> &data)
{
	ROutMessage *mess = new ROutMessage;
	mess->id = seqId;
	mess->parent = this;
	mess->data = data;
	mess->ackReceived = false;
	mess->ackCond = new Semaphore("ack condvar", 0);

	mOutMessages[seqId] = mess;
	SendData(seqId, data);

	seqId = (seqId % INT32_MAX) + 1;

	interrupt->Schedule(ProcAckSem, (int)mess->ackCond, REEMISSION_DELAY * 200, NetworkSendInt);
	mess->ackCond->Wait();
	for (int i = 0; i < MAX_REEMISSIONS; i++) {
		if (mess->ackReceived) {
			mess->parent->mOutMessages.erase(mess->id);
			// delete mess->ackCond;
			// delete mess;
			return;
		} else {
			DEBUG('n', "Trying to send message\n");
			mess->transmissionCount++;
			mess->parent->SendData(mess->id, mess->data);
			interrupt->Schedule(ProcAckSem, (int)mess->ackCond, REEMISSION_DELAY * 100, NetworkSendInt);
		}
	}

	if (!mess->ackReceived) {
		DEBUG('n', "Failed sending message after %d tries\n", MAX_REEMISSIONS);
	}
}

void RConn::Receive(int size, char *data)
{
	while (!mInMessages.count(friendSeqId)) {
		currentThread->Yield();
	}

	RInMessage *in = mInMessages[friendSeqId];
	mInMessages.erase(friendSeqId);

	memcpy(data, in->data.data(), in->data.size());

	delete in;
	friendSeqId = (friendSeqId % INT32_MAX) + 1;
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
	ASSERT(data.size() < MaxMailSize - sizeof(RHeader));

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

	while (1) {
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
}