/*
	A humble packet
*/

// This is the basic structure of a packet.
struct Packet
{
	// Headers
	int seq;
	int source;
	int destination;
	int type;
	int relayed;
	
	// Payload
	double timestamp;
	int payload;

	// NOT INCLUDED IN THE PACKET!
	int isBusy;
};
