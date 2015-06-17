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
chmod +x ../script/caliper
chmod +x ../script/computeAlpha
cd ..

echo "Compiling Hotspot..."
cd hotspot
make
cp hotspot ../script
chmod +x ../script/hotspot
cd ..

echo "Compiling Hotspot_Gui..."
cd Hotspot_Gui
qmake Hotspot_Gui.pro
make
chmod +x Hotspot_Gui

echo "Finished compilation"
