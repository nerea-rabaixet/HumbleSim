/*
	FIFO
*/

#include <deque>

component FIFO : public TypeII
{	
	private:
		std::deque <Packet> myQueue;
		
	public:
		Packet GetFirstPacket();
		Packet GetPacketAt(int n);
		void DelFirstPacket();		
		void DeletePacketIn(int i);
		void PutPacket(Packet &packet);	
		void PutPacketFront(Packet &packet);	
		void PutPacketIn(Packet &packet, int);	
		int QueueSize();
};

Packet FIFO :: GetFirstPacket()
{
	return(myQueue.front());	
}; 

Packet FIFO :: GetPacketAt(int n)
{
	return(myQueue.at(n));	
}; 

void FIFO :: DelFirstPacket()
{
	myQueue.pop_front();
}; 

void FIFO :: PutPacket(Packet &packet)
{	
	myQueue.push_back(packet);
}; 

void FIFO :: PutPacketFront(Packet &packet)
{	
	myQueue.push_front(packet);
}; 

int FIFO :: QueueSize()
{
	return(myQueue.size());
}; 

void FIFO :: PutPacketIn(Packet & packet,int i)
{
	myQueue.insert(myQueue.begin()+i,packet);
}; 

void FIFO :: DeletePacketIn(int i)
{
	myQueue.erase(myQueue.begin()+i);
};
