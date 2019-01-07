#include "rconn.h"

#include "post.h"
#include <system.h>

void ReceiveAck(int conn);

RConn::RConn(PostOffice *post, int to_addr, int mailboxId) : mPost(post), addr(to_addr), mailbox(mailboxId)
{
	seqId = 1;
	friendSeqId = 1;

	Thread *t = new Thread("ack receiver");

	t->Fork(Receive, (int)this);
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
	mess->transmissionCount = 1;
	mess->ackReceived = false;

	mOutMessages[seqId] = mess;
	SendData(seqId, data);
	interrupt->Schedule(RConn::CheckAck, (int)mess, REEMISSION_DELAY * 2, NetworkSendInt);
	seqId = (seqId % INT32_MAX) + 1;
}

void RConn::Receive(int size, char *data)
{
	while (!mInMessages.count(friendSeqId)) {
		;
	}

	RInMessage *in = mInMessages[friendSeqId];
	mInMessages.erase(friendSeqId);

	memcpy(data, in->data.data(), in->data.size());

	delete in;
	friendSeqId = (friendSeqId % INT32_MAX) + 1;
}

void RConn::CheckAck(int messageAddr)
{
	ROutMessage *mess = (ROutMessage *)messageAddr;

	if (mess->ackReceived || mess->transmissionCount == MAX_REEMISSIONS) {
		if (!mess->ackReceived) {
			DEBUG('n', "Failed sending message after %d tries", MAX_REEMISSIONS);
		}

		mess->parent->mOutMessages.erase(mess->id);
		delete mess;
	} else {
		mess->transmissionCount++;
		mess->parent->SendData(mess->id, mess->data);
		interrupt->Schedule(CheckAck, messageAddr, REEMISSION_DELAY, NetworkSendInt);
	}
}

void RConn::SendAck(SeqId id)
{
	PacketHeader pktHdr;
	MailHeader mailHdr;

	pktHdr.to = addr;
	mailHdr.to = mailbox;
	mailHdr.length = sizeof(RHeader);

	mPost->Send(pktHdr, mailHdr, (const char *)&id);
}

void RConn::SendData(SeqId id, const std::vector<char> &data)
{
	ASSERT(data.size() < MaxMailSize - sizeof(RHeader));

	PacketHeader pktHdr;
	MailHeader mailHdr;

	pktHdr.to = addr;
	mailHdr.to = mailbox;
	mailHdr.length = sizeof(RHeader) + data.size();

	char withHeader[MaxMailSize];
	memcpy(withHeader, &id, sizeof(SeqId));
	memcpy(withHeader + sizeof(SeqId), data.data(), data.size());

	mPost->Send(pktHdr, mailHdr, withHeader);
}

void RConn::Receive(int con)
{
	RConn *conn = (RConn *)con;
	PostOffice *post = conn->mPost;

	char data[MaxMailSize];

	while (1) {
		PacketHeader pktH;
		MailHeader mailH;
		post->Receive(conn->mailbox, &pktH, &mailH, data);

		if (mailH.length > sizeof(RHeader)) {
			RHeader *hdr = (RHeader *)data;

			if (hdr->id < 0) // Ack
			{
				SeqId realSeqId = -hdr->id;
				if (conn->mOutMessages.count(realSeqId)) {
					conn->mOutMessages[realSeqId]->ackReceived = true;
				}
			} else // Message
			{
				conn->SendAck(hdr->id);

				if (conn->mInMessages.count(conn->friendSeqId) == 0) {
					RInMessage *in = new RInMessage;
					in->parent = conn;
					in->id = conn->friendSeqId;
					in->data = std::vector<char>(mailH.length - sizeof(RHeader), 0);
					memcpy(in->data.data(), data + sizeof(RHeader), mailH.length - sizeof(RHeader));
					conn->mInMessages[conn->friendSeqId] = in;
				}
			}
		} else {
			DEBUG('n', "Malformed reliable packet");
		}
	}
}