
//A humble packet
// This is the basic structure of a packet.
// Copyright (c) 2023, Universitat Pompeu Fabra.
//
// This file is provided "as is" without warranty of any kind. Please see the file LICENCE for more details.
//
// Authors: Nerea Rabaixet and Costas Michaelides
// Date: 2023-6-26

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
	//indicator --> available for relaying

	// NOT INCLUDED IN THE PACKET!
	int isBusy;
	//is able to be a Relay
	int ableToRelay;
	//agregation packet
	int aggregation;
	int source_agg;
};
