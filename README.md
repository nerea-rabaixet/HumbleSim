# HumbleSim
The simulation reproduces the behavior of a Class B LoRaWAN network system with the implementation of relaying, it is written in C++ using the COST framework which is a compact framework for discrete simulations (DES). So, our simulation consists of a number of events, executed sequentially. 

Our simulation is focused on medium access control (MAC) We set an area where the area is 5000 x 5000 meters. It randomly generates the positions of the nodes in a 5x5 km area and we put the Gateway to the center of the square space. The gateway threashold is set to 2,5km.
