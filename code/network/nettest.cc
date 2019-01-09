// nettest.cc
//	Test out message delivery between two "Nachos" machines,
//	using the Post Office to coordinate delivery.
//
//	Two caveats:
//	  1. Two copies of Nachos must be running, with machine ID's 0 and 1:
//		./nachos -m 0 -o 1 &
//		./nachos -m 1 -o 0 &
//
//	  2. You need an implementation of condition variables,
//	     which is *not* provided as part of the baseline threads
//	     implementation.  The Post Office won't work without
//	     a correct implementation of condition variables.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "interrupt.h"
#include "network.h"
#include "post.h"
#include "rconn.h"
#include "system.h"

// Test out message delivery, by doing the following:
//	1. send a message to the machine with ID "farAddr", at mail box #0
//	2. wait for the other machine's message to arrive (in our mailbox #0)
//	3. send an acknowledgment for the other machine's message
//	4. wait for an acknowledgement from the other machine to our
//	    original message

void MailTest(int farAddr)
{
	PacketHeader outPktHdr, inPktHdr;
	MailHeader outMailHdr, inMailHdr;
	const char *data = "Hello there!";
	const char *ack = "Got it!";
	char buffer[MaxMailSize];

	for (int i = 0; i < 10; ++i) {
		// construct packet, mail header for original message
		// To: destination machine, mailbox 0
		// From: our machine, reply to: mailbox 1
		outPktHdr.to = farAddr;
		outMailHdr.to = 1;
		outMailHdr.from = 1;
		outMailHdr.length = strlen(data) + 1;

		// Send the first message
		postOffice->Send(outPktHdr, outMailHdr, data);

		// Wait for the first message from the other machine
		postOffice->Receive(1, &inPktHdr, &inMailHdr, buffer);
		printf("Got \"%s\" from %d, box %d\n", buffer, inPktHdr.from, inMailHdr.from);
		fflush(stdout);

		// Send acknowledgement to the other machine (using "reply to" mailbox
		// in the message that just arrived
		outPktHdr.to = inPktHdr.from;
		outMailHdr.to = inMailHdr.from;
		outMailHdr.length = strlen(ack) + 1;
		postOffice->Send(outPktHdr, outMailHdr, ack);

		// Wait for the ack from the other machine to the first message we sent.
		postOffice->Receive(1, &inPktHdr, &inMailHdr, buffer);
		printf("Got \"%s\" from %d, box %d\n", buffer, inPktHdr.from, inMailHdr.from);
		fflush(stdout);
	}

	// Then we're done!
	interrupt->Halt();
}

void RConnTest(int farAddr)
{
	RConn *conn = new RConn(postOffice, farAddr, 2);

	std::string str("Hello World!");
	conn->send(str.length() + 1, str.c_str());
	printf("Sent #1\n");

	str = "Hell World!";
	conn->send(str.length() + 1, str.c_str());
	printf("Sent #2\n");

	str = "Hell Lord!";
	conn->send(str.length() + 1, str.c_str());
	printf("Sent #3\n");

	char data[MaxMailSize] = {'\0'};

	conn->recv(0, data);
	printf("Got %s\n", data);

	conn->recv(0, data);
	printf("Got %s\n", data);

	conn->recv(0, data);
	printf("Got %s\n", data);

	for (int i = 0; i < 10000; ++i)
		currentThread->Yield();

	conn->close();

	// Then we're done!
	interrupt->Halt();
}
