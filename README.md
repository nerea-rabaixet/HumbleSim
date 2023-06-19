# HumbleSim
The simulation reproduces the behavior of a Class B LoRaWAN network system with the implementation of relaying, it is written in C++ using the COST framework which is a compact framework for discrete simulations (DES). So, our simulation consists of a number of events, executed sequentially. 

Our simulation is focused on medium access control (MAC) We set an area where the area is 5000 x 5000 meters. It randomly generates the positions of the nodes in a 5x5 km area and we put the Gateway to the center of the square space. The gateway threashold is set to 2,5km.

## Compile: 
  chmod +x build_local COST/cxx; LD_PRELOAD= ./build_local

## Run:
  ./HumbleSim 3600 1 10 666 1
  
  As input values we have: Sim time, relaying enabler, number of nodes, seed and packet aggregation enabler

Authors:
  Costas Michaelades
  
  Nerea Rabaixet
