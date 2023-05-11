/*
	A humble node
*/
#include <stdio.h>      /* printf, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

// Packet types
#define BEACON 0
#define PING 1
#define DATA 2
#define SENSE 3

// Periodicity
#define BEACON_PERIOD 128 // Beacon period in seconds/ maybe should depend on num of nodes
#define PING_PERIOD 16 // Ping period in seconds

component Node : public TypeII
{
	public:
		void Start();
		void Stop();

	public:
		int id;
		int isGateway;	// in this sim Gateway = Node 0
		int isConnected; // A node considers itself connected when it receives a beacon
		int seqNum; // Sequence number
		int nodes;
		int seed;
		int application;
		int maxNumRelayingperBeacon; //max number of relaying per beacon period
		int collectTraces;
		int myRelayNode; // The relay (connected) node
		int myRelayedNode; // The relayed (disconnected) node
		int isRelayingEnabled;
		int RelayNodeIsEnable;
		float myBackoff;
		float positionX;
		float positionY;
		Packet NewPacket(int type); //create new packet

	private:
		char msg[500];
		int pings;
		int maxPings;

	private:
		int transmittedBeacons;
		int transmittedPings;
		int transmittedData;
		int receivedBeacons;
		int receivedPings;
		int receivedData;
		//relaying info
		int busyretry; // retries of sensing, set to 0 when it is finaly transmited
		int isolatedData; // Transmitted data packets from relayed node
		int isolatedDataperNode;
		int relayedData; // Transmitted data packets from relay node
		//max relayed nodes


	public:
		// Connections
		inport void Rx(Packet &packet);
		outport void Tx(Packet &packet);
		outport void Trace(char* input);
		outport void Result(char* input);

		// Timers
		Timer <trigger_t> superframe; // Triggers the transmission of the next Beacon
		Timer <trigger_t> ping; // Triggers the transmission of the next Ping
		Timer <trigger_t> data; // Triggers the transmission of the next Data packet
		Timer <trigger_t> sense; // Triggers channel sensing

		inport void Beacon(trigger_t& t1);
		inport void Ping(trigger_t& t2);
		inport void Data(trigger_t& t3);
		inport void Sense(trigger_t& t4);

		Node()
		{
			connect superframe.to_component,Beacon;
			connect ping.to_component,Ping;
			connect data.to_component,Data;
			connect sense.to_component,Sense;
		}
};

void Node :: Start()
{
	//initial values
	seqNum = 0; // This is the sequence number of my first packet
	myRelayNode = 0;
	myRelayedNode = 0;
	myBackoff = 0.001; //backoff is that after a collision detection, the devices that were transmitting will transmit a jam signal to inform all hosts that a collision has occurred.
	isConnected = 0; //all nodes are initially set as not connected

	// Statistics!
	transmittedBeacons = 0;
	transmittedPings = 0;
	transmittedData = 0;
	receivedBeacons = 0;
	receivedPings = 0;
	receivedData = 0;
	isolatedData = 0;
	isolatedDataperNode = 0;
	relayedData = 0;

	sprintf(msg,"%f - Node %d: My coordinates are (%f,%f).",SimTime(),id,positionX,positionY);
	if (collectTraces) Trace(msg);

	if (isGateway) {
		superframe.Set(SimTime()); // Start running!
	}
};

void Node :: Stop()
{
	//print Statistics
	printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",seed,nodes,id,(int)positionX,(int)positionY,transmittedBeacons,receivedBeacons,transmittedPings,receivedPings,transmittedData,receivedData,isolatedData,relayedData);
	sprintf(msg,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",seed,nodes,id,(int)positionX,(int)positionY,transmittedBeacons,receivedBeacons,transmittedPings,receivedPings,transmittedData,receivedData,isolatedData,relayedData);
	Result(msg);
};

// Transmission of a Beacon packet.
// Note that this is triggered by a Timer.
void Node :: Beacon(trigger_t &){
	Packet packet = NewPacket(BEACON); //create packet type Beacon
	Tx(packet);
	transmittedBeacons++;
	sprintf(msg,"%f - Node %d: I transmitted a Beacon",SimTime(),id);
	if (collectTraces) Trace(msg);
	superframe.Set(SimTime()+BEACON_PERIOD); // Schedules the next Beacon
	pings = 0;
	maxPings = BEACON_PERIOD/PING_PERIOD - 1;
	ping.Set(SimTime()+PING_PERIOD); // Schedules the next Ping
};

// Transmission of a Ping packet.
// Note that this is triggered by a Timer.
void Node :: Ping(trigger_t &){
	pings++;
	if (pings < maxPings){
		Packet packet = NewPacket(PING); //create packet type Ping
		packet.destination = 1+Random(nodes-1); //assign destination at random
		Tx(packet);
		transmittedPings++;
		sprintf(msg,"%f - Node %d: I transmitted a Ping to Node %d.",SimTime(),id,packet.destination);
		if (collectTraces) Trace(msg);
		ping.Set(SimTime()+PING_PERIOD); // Schedules the next Ping
	}
};

// Transmission of a Data packet.
// Note that this is triggered by a Timer.
void Node :: Data(trigger_t &){
	Packet packet = NewPacket(DATA);
	if (myRelayNode != 0) {	//If I have a RelayNode
		packet.destination = myRelayNode; //destinatio is the Relay
		myRelayNode = 0;
		isolatedData++;

	} else if (myRelayedNode != 0) { // If I am the relay of another node
		packet.relayed = myRelayedNode;
		myRelayedNode = 0;

		if(isolatedDataperNode < maxNumRelayingperBeacon){ //when I have relayed at lest 2 packets then I stop tranmitting
			isolatedDataperNode++;
			relayedData++;
			sprintf(msg,"%f - Node %d: My isolated Data per Node is %d.",SimTime(),id,isolatedDataperNode);
			if (collectTraces) Trace(msg);
		}
		else{
			RelayNodeIsEnable = 0; //resets relaying again = 0;
		}
	}
	packet.ableToRelay = RelayNodeIsEnable; //0 if the Node is not able to relay more packets.
	Tx(packet);
	transmittedData++;

	sprintf(msg,"%f - Node %d: I transmitted a Data packet to Node %d.",SimTime(),id,packet.destination);
	if (collectTraces) Trace(msg);
};

void Node :: Sense(trigger_t &){
	Packet packet = NewPacket(SENSE);
	Tx(packet);
}

// This function filters the received packets.
// We decide either to use a packet or drop it!
void Node :: Rx(Packet &packet)
{
	// Is this packet for me?

	// First, let's check that it was not transmitted by me!
	if (id != packet.source){

		// If the packet is broadcasted, then let's check what it is about!
		if (packet.destination == -1){
			if (packet.type == BEACON){
				receivedBeacons++;
				isConnected = 1; // If you receive a beacon then you know you are connected
				RelayNodeIsEnable = 1; //resets relaying again enable
				isolatedDataperNode = 0;//set again the counter of isolatedDataper Node to 0 so that we can transmit a max of 2 packets.
				sprintf(msg,"%f - Node %d: I received a Beacon and I am connected!",SimTime(),id);
				if (collectTraces) Trace(msg);

			}
			else
			{
				sprintf(msg,"%f - Node %d: I received an unknown broadcasted packet.",SimTime(),id);
				if (collectTraces) Trace(msg);
			}
		}
		// What if the packet is not broadcasted?
		else{
			// This packet is for me!
			if (packet.destination == id){
				if (packet.type == PING){
					receivedPings++;
					sprintf(msg,"%f - Node %d: I received a Ping from Node %d.",SimTime(),id,packet.source);
					if (collectTraces) Trace(msg);
					data.Set(SimTime()); // Schedules the transmission of the data packet
				}

				else {
					if (packet.type == DATA){
						receivedData++;
						sprintf(msg,"%f - Node %d: I received a Data packet from Node %d.",SimTime(),id,packet.source);
						if (collectTraces) Trace(msg);

						// Relaying! Check if it needs to be relayed to GW
						if (id != 0 && packet.destination == id && isConnected && RelayNodeIsEnable == 1){
							myRelayedNode = packet.source;
							sprintf(msg,"%f - Node %d: I received a Data packet from Node %d and I will forward it to the GW. [relaying]",SimTime(),id,packet.source);
							if (collectTraces) Trace(msg);
							data.Set(SimTime()+Exponential(myBackoff));
						}
					}
				}
			}
			// This packet is not for me :(
			else {
				sprintf(msg,"%f - Node %d: I received a packet from Node %d, but it's not for me.",SimTime(),id,packet.source);
				if (collectTraces) Trace(msg);

				// I hear someone that has connection to GW I will ask him to be my relay.
				if (isRelayingEnabled && !isConnected && packet.destination == 0 && packet.ableToRelay != 0){
					myRelayNode = packet.source; // I want this to be my relay node!
					sprintf(msg,"%f - Node %d: I will try to transmit to Node %d soon. [relaying]",SimTime(),id,myRelayNode);
					if (collectTraces) Trace(msg);
					sense.Set(SimTime()+Exponential(myBackoff));
				}
			}
		}
	}

	// Here, we receive an answer from the channel (whether it's busy or not).
	// This is not a real transmission!
	if(id == packet.source && packet.type == SENSE){
		if(packet.isBusy == 0) {
			sprintf(msg,"%f - Node %d: The channel is not busy!",SimTime(),id);
			if (collectTraces) Trace(msg);
			data.Set(SimTime());
			busyretry = 0;
		} else {
			busyretry ++;
			sprintf(msg,"%f - Node %d: The channel is busy!",SimTime(),id);
			if (collectTraces) Trace(msg);
		}
	}
};

// This is a function to create new packets
Packet Node :: NewPacket(int type) {

	Packet packet;

	switch(type)
	{
		case BEACON:
			packet.seq = 0;
			packet.source = id;
			packet.destination = -1; // Broadcast
			packet.type = BEACON;
			packet.timestamp = SimTime();
			break;
		case PING:
			packet.seq = 0;
			packet.source = id;
			packet.destination = 0;
			packet.type = PING;
			packet.timestamp = SimTime();
			break;
		case DATA:
			seqNum++;
			packet.seq = seqNum;
			packet.source = id;
			packet.destination = 0;
			packet.relayed = 0;
			packet.type = DATA;
			packet.timestamp = SimTime();
			packet.ableToRelay = 1;
			break;
		case SENSE:
			packet.seq = 0;
			packet.source = id;
			packet.destination = myRelayNode;
			packet.type = SENSE;
			packet.timestamp = SimTime();
			break;
	}

	return packet;
};
