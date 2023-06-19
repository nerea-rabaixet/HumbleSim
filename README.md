# HumbleSim
The simulation reproduces the behavior of a Class B LoRaWAN network system with the implementation of relaying, it is written in C++ using the COST framework which is a compact framework for discrete simulations (DES). So, our simulation consists of a number of events, executed sequentially. 

Our simulation is focused on medium access control (MAC) We set an area where the area is 5000 x 5000 meters. It randomly generates the positions of the nodes in a 5x5 km area and we put the Gateway to the center of the square space. The gateway threashold is set to 2,5km.

As we are in a discrete event simulation, here we generate the beacon packets from the gateway with a periodicity of 128 seconds. Pings are transmitted every 16 seconds from the gateway to a random node. Data packets are randomly generated and they are stored in a FIFO queue waiting to be transmitted.

As input values we have: Sim time, relaying enabler, number of nodes, seed and packet aggregation enabler

Compile: !mypath="drive/MyDrive/TFG"; cd $mypath/HumbleSim; chmod +x build_local COST/cxx; LD_PRELOAD= ./build_local

Run: ./HumbleSim 3600 1 10 666 1
