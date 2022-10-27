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
	double timestamp;
	int relay; //node that is relaying
	int iso; //node that is isolated
	int isBusy;

	// Payload
	int payload;
};
