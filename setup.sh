#!/bin/bash 

echo "Create directory"
mkdir data
mkdir result
mkdir script

echo "Compiling Caliper..."
cd caliper
make
cp caliper ../script
cp computeAlpha ../script
cd ..

echo "Compiling Hotspot..."
cd hotspot
make
cp hotspot ../script
cd ..

echo "Compiling Hotspot_Gui..."
cd Hotspot_Gui
qmake Hotspot_Gui.pro
make

echo "Finished compilation"
