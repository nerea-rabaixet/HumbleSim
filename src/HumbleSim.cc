#include <iostream>
#include <random>

#include "./COST/cost.h"
#include "Node.h"
#include "Channel.h"
#include "Packet.h"
#include "Logger.h"

using namespace std;

component HumbleSim : public CostSimEng
{
	public:
		void Setup(int nodes, int seed, int collectTraces, int isRelayingEnabled, int maxNumRelayingperBeacon, int PutRelayPacketFront);
		void Stop();
		void Start();

	public:
		Channel channel;
		Node [] node;
		Logger log;

	private:
		int sizeX;
		int sizeY;
		char msg[500];
};

void HumbleSim:: Setup(int nodes, int seed, int collectTraces, int isRelayingEnabled,int maxNumRelayingperBeacon, int PutRelayPacketFront)
{
	node.SetSize(nodes);

	channel.Out.SetSize(nodes);
	channel.nodes = nodes;
	channel.nodePositionX = new float [nodes];
	channel.nodePositionY = new float [nodes];
	channel.threshold = 2500; // in meters
	channel.collectTraces = collectTraces;

	// Connection
	connect channel.Trace,log.Trace;

	// Setup End Devices
	for(int i=0;i<nodes;i++)
	{
		sizeX = 5000; // in meters
		sizeY = 5000; // in meters

		//random_device rand_dev;
		//mt19937 generator(rand_dev());
		mt19937 generator(seed*i);
		uniform_int_distribution<int> distrX(0, sizeX);
		uniform_int_distribution<int> distrY(0, sizeY);

		float positionX = distrX(generator);
		float positionY = distrY(generator);

		node[i].id = i;
		node[i].isGateway = 0;
		node[i].nodes = nodes;
		node[i].positionX = positionX;
		node[i].positionY = positionY;
		node[i].collectTraces = collectTraces;
		node[i].isRelayingEnabled = isRelayingEnabled;
		node[i].PutRelayPacketFront = PutRelayPacketFront;
		node[i].seed = seed;
		node[i].application = isRelayingEnabled;
		node[i].maxNumRelayingperBeacon = maxNumRelayingperBeacon;

		channel.nodePositionX[i] = positionX;
		channel.nodePositionY[i] = positionY;

		// Connections
		connect node[i].Tx,channel.In;
		connect channel.Out[i],node[i].Rx;
		connect node[i].Trace,log.Trace;
		connect node[i].Result,log.Result;
	}

	// Setup Gateway
	node[0].isGateway = 1;
	node[0].nodes = nodes;
	node[0].seed = seed;
	node[0].positionX = sizeX/2;
	node[0].positionY = sizeY/2;
	channel.nodePositionX[0] = sizeX/2;
	channel.nodePositionY[0] = sizeY/2;
};

void HumbleSim:: Start()
{
	printf("--- Started ---------------------------------------------------------------\n");
	printf("Seed\tApp\tID\tPosX\tPosY\tBeacTX\tBeacRX\tPingTX\tPingRX\tDataTX\tDataRX\tIsolTX\tRelayTX\tDropped\taverageLatency\n");
};

void HumbleSim:: Stop()
{
	printf("--- Finished --------------------------------------------------------------\n");
};

int main(int argc, char *argv[])
{
	if(argc < 8)
	{
		printf("./HumbleSim simTime collectTraces isRelayingEnabled nodes seed maxNumRelayingperBeacon PutRelayPacketFront\n");
		return 0;
	}

	double simTime = atof(argv[1]);
	int collectTraces = atoi(argv[2]);
	int isRelayingEnabled = atoi(argv[3]);
	int nodes = atoi(argv[4]);
	int seed = atoi(argv[5]);
	int maxNumRelayingperBeacon = atoi(argv[6]);
	int PutRelayPacketFront = atoi(argv[7]);

	HumbleSim sim;

	printf("--- Humble LoRaWAN Simulator ----------------------------------------------\n");

	//sim.Seed=(long int)6*rand();
	sim.StopTime(simTime);
	sim.Setup(nodes,seed,collectTraces,isRelayingEnabled, maxNumRelayingperBeacon,PutRelayPacketFront);
	sim.Run();

	return(0);
};
