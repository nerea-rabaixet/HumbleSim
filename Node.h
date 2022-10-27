/*
	A humble node
*/

// Packet types
#define BEACON 0
#define PING 1
#define DATA 2
#define TEST 3

// Periodicity
#define BEACON_PERIOD 128 // Beacon period in seconds
#define PING_PERIOD 8 // Ping period in seconds
#define DATA_PERIOD 20 // Data period in seconds

component Node : public TypeII
{
	public:
		void Start();
		void Stop();

	public:
		int id;
		int isGateway;
		int Connected; // node is not connected by default
		int relay; //relay of an isolated node
		int relayed; //to resend data to GW
		int seqNum;
		int nodes;
		int seed;
		int collectTraces;
		float positionX;
		float positionY;
		Packet NewPacket(int type);
		int is_busy;

	private:
		char msg[80];
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
		int packetRelay;
		int packetRelayed;

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

		inport void Beacon(trigger_t& t1);
		inport void Ping(trigger_t& t2);
		inport void Data(trigger_t& t3);

		Node()
		{
			connect superframe.to_component,Beacon;
			connect ping.to_component,Ping;
			connect data.to_component,Data;
			connect test.to_component,Test;
		}
};

void Node :: Start()
{
	// This is the sequence number of my first packet
	seqNum = 0;

	// Statistics!
	transmittedBeacons = 0;
	transmittedPings = 0;
	transmittedData = 0;
	receivedBeacons = 0;
	receivedPings = 0;
	receivedData = 0;
	packetRelay = 0;
	packetRelayed = 0;

	sprintf(msg,"%f - Node %d: My coordinates are (%f,%f).",SimTime(),id,positionX,positionY);
	if (collectTraces) Trace(msg);

	if (isGateway) {
		superframe.Set(SimTime()); // Start running!
	}
};

void Node :: Stop()
{
	printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",id,(int)positionX,(int)positionY,transmittedBeacons,receivedBeacons,transmittedPings,receivedPings,transmittedData,receivedData,packetRelay,packetRelayed);
	sprintf(msg,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",id,(int)positionX,(int)positionY,transmittedBeacons,receivedBeacons,transmittedPings,receivedPings,transmittedData,receivedData,packetRelay,packetRelayed);
	Result(msg);
};

// Transmission of a Beacon packet.
// Note that this is triggered by a Timer.
void Node :: Beacon(trigger_t &)
{
	Packet packet = NewPacket(BEACON);
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
void Node :: Ping(trigger_t &)
{
	pings++;

	if (pings < maxPings)
	{
		Packet packet = NewPacket(PING);

		mt19937 generator(seed*transmittedPings);
		uniform_int_distribution<int> distr(1, nodes-1);
		packet.destination = distr(generator);

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
	if(Connected == 1){
		Packet packet = NewPacket(DATA);
		if(relayed != 0){
			packet.destination = 0;
			packet.source = id;
			packet.iso = relayed;
		}
		
		Tx(packet);
		transmittedData++;
	}
	else{
		Packet packet = NewPacket(DATA);
		packet.relay = relay;
		packet.destination = relay; //request relay
		packet.iso = id; //Indicate which is the isloated node
		packet.source = id; //put the source as the relay
		Tx(packet);
		transmittedData++;
		sprintf(msg,"%f - Node %d: I request Relay to node %d ",SimTime(),id,packet.destination);
		if (collectTraces) Trace(msg);
		}

	sprintf(msg,"%f - Node %d: I transmitted a Data packet",SimTime(),id);
	if (collectTraces) Trace(msg);
};


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
				Connected = 1; // If you receive a beacon then you know you are connected
				sprintf(msg,"%f - Node %d: I received a Beacon and I am connected!.",SimTime(),id);
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
					}
				//check if it needs to be relayed to GW
					if (packet.relay == id && id != 0){
					relayed = packet.iso; // we set the packet to be directed to GW
					int random_value = rand()%10;
					data.Set(SimTime()+random_value); //send Data packet forwarded to Node 0 (GW)
					packetRelay++;
					sprintf(msg,"%f - Node %d: I RELAY received packet from Node %d, I will forward it to GW.",SimTime(),id,packet.source);
					if (collectTraces) Trace(msg);
					transmittedData++;
					}
				}
			}
			// This packet is not for me :(
			else if(Connected == 0){ //Node is isolated and receves a Data packet
				
				relay = packet.source; //indicate the relay to use
				int random_value = rand()%10;
				data.Set(SimTime()+random_value); //send packet to the Relay Node.
				packetRelayed++;
				sprintf(msg,"%f - Node %d: I received a packet from Node %d, I ask him to Relay me.",SimTime(),id,packet.source);
				if (collectTraces) Trace(msg);
				transmittedData++;

			}
			else {
				sprintf(msg,"%f - Node %d: I received a packet from Node %d, but it's not for me.",SimTime(),id,packet.source);
				if (collectTraces) Trace(msg);
			}
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
			packet.type = DATA;
			packet.timestamp = SimTime();
			break;
	}

	return packet;
};
