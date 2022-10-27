/*
	A humble wireless channel
*/

// Packet types
#define BEACON 0
#define PING 1
#define DATA 2
#define TEST 3

// Airtime (SF9/125kHz)
// https://avbentem.github.io/airtime-calculator/ttn/eu868
#define TX_HEADERS 1649E-4 // TX time for 13 Bytes (only headers) in seconds
#define TX_UPLINK 2058E-4 // TX time of 25 Bytes (headers and payload) in seconds

component Channel : public TypeII
{
	public:
		void Start();
		void Stop();

	private:
		char msg[80];
		int isActive;  // It can be 0 or 1!

	public:
		int nodes;
		float *nodePositionX;
		float *nodePositionY;
		Packet currentPacket;
		int threshold;
		int collectTraces;

		// Connections
		inport void In(Packet &packet);
		outport [] void Out(Packet &packet);
		outport void Trace(char* test);

		// Timers
		Timer <trigger_t> airtime;
		inport void Travel(trigger_t& t1);

		Channel()
		{
			connect airtime.to_component,Travel;
		}
};

void Channel :: Start()
{
	isActive = 0;
};

void Channel :: Stop()
{
	delete [] nodePositionX;
	delete [] nodePositionY;
};

// We take into account the airtime of a packet and schedule a timer to decide what to do.
// Please note that we do not allow any simultaneous transmissions in our channel!
void Channel :: In(Packet &packet){
	if (!isActive) { // We make sure that the channel is not active
		isActive = 1; // We set the channel as active!
		currentPacket = packet;

		switch(packet.type) {
			case BEACON:
				airtime.Set(SimTime()+TX_HEADERS);
				break;
			case PING:
				airtime.Set(SimTime()+TX_HEADERS);
				break;
			case DATA:
				airtime.Set(SimTime()+TX_UPLINK);
				break;

		}
	}
	
	else {
		airtime.Cancel();
		sprintf(msg, "%f - Channel: Collision detected from %d!", SimTime(),packet.source);
		if (collectTraces) Trace(msg);
	}
};

// We allow a packet to travel to a node if the distance is less than a threshold.
// Note that this is triggered by a Timer.
void Channel :: Travel(trigger_t& t1)
{
	float distance;
	int source = currentPacket.source;
	//check if it is a test packets

		for(int i=0;i<nodes;i++) {
			// We use the pythagorean theorem to calculate the distance of two points in 2D space.
			distance = sqrt(pow(nodePositionX[i]-nodePositionX[source],2) + pow(nodePositionY[i]-nodePositionY[source],2));

			if (distance < threshold) {
				Out[i](currentPacket); // So, we allow the packet to travel!
			}
			else {
				//we set the node as not reachable
				sprintf(msg, "%f - Channel: Node %d is out of range!", SimTime(), i);
				if (collectTraces) Trace(msg);
			}

	  }
	isActive = 0; // Now the channel is inactive again!
};
