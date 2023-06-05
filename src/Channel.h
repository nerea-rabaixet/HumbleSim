/*
	A humble wireless channel
*/

// Packet types
#define BEACON 0
#define PING 1
#define DATA 2
#define SENSE 3

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
		char msg[500];
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
		outport void Trace(char* msg);

		// Timers
		Timer <trigger_t> airtime;
		Timer <trigger_t> collision;
		inport void Travel(trigger_t& t1);
		inport void DeactivateChannel(trigger_t& t1);

		Channel()
		{
			connect airtime.to_component,Travel;
			connect collision.to_component,DeactivateChannel;
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
	if (packet.type == SENSE) {
		if(isActive) {
			packet.isBusy = 1;
		}
		else {
			packet.isBusy = 0;
		}
		Out[packet.source](packet);
	}
	else if (!isActive) { // We make sure that the channel is not active
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
				if(packet.aggregation == 1){
					airtime.Set(SimTime()+2*TX_UPLINK);
				}
				else{
					airtime.Set(SimTime()+TX_UPLINK);
				}

				break;
		}
	}
	else {
		airtime.Cancel();
		sprintf(msg, "%f - Channel: Node %d caused a collision!", SimTime(),packet.source);
		if (collectTraces) Trace(msg);
		collision.Set(SimTime()+TX_UPLINK);
	}
};

// We allow a packet to travel to a node if the distance is less than a threshold.
// Note that this is triggered by a Timer.
void Channel :: Travel(trigger_t& t1)
{
	float distance;
	int source = currentPacket.source;

	for(int i=0;i<nodes;i++) {
		// We use the pythagorean theorem to calculate the distance of two points in 2D space.
		distance = sqrt(pow(nodePositionX[i]-nodePositionX[source],2) + pow(nodePositionY[i]-nodePositionY[source],2));

		if (distance < threshold) {
			Out[i](currentPacket); // So, we allow the packet to travel!
		}
		else {
			// The node is not reachable.
			sprintf(msg, "%f - Channel: Node %d is out of range!", SimTime(), i);
			if (collectTraces) Trace(msg);
		}
	}

	isActive = 0; // Now the channel is inactive again!

};

// This function deactivates the channel.
// Note that this is triggered by a Timer.
void Channel :: DeactivateChannel(trigger_t& t1)
{
	isActive = 0;
};
