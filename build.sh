#!/bin/sh

# Replace this with the path to the DAQ release
RELEASE=/srv/nfs/lsst-daq/daq-sdk/current

g++ -o read_gds.o -c -std=gnu++0x -g -Wall -DSINGLE_TASK -I${RELEASE}/include read_gds.cc
g++ -o read_gds read_gds.o -Wl,-rpath=${RELEASE}/x86/lib -L${RELEASE}/x86/lib -ldaq_daq -ldaq_ims -ldaq_dsi

g++ -o read_wfs.o -c -std=gnu++0x -g -Wall -DSINGLE_TASK -I${RELEASE}/include read_wfs.cc
g++ -o read_wfs read_wfs.o -Wl,-rpath=${RELEASE}/x86/lib -L${RELEASE}/x86/lib -ldaq_daq -ldaq_ims -ldaq_dsi

g++ -o read_sci.o -c -std=gnu++0x -g -Wall -DSINGLE_TASK -I${RELEASE}/include read_sci.cc
g++ -o read_sci read_sci.o -Wl,-rpath=${RELEASE}/x86/lib -L${RELEASE}/x86/lib -ldaq_daq -ldaq_ims -ldaq_dsi
