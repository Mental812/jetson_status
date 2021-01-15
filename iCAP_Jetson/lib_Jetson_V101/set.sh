#!/bin/bash
sudo apt-get update
sudo apt-get install python3-pip
sudo pip3 install --upgrade pip jetson_stats


if [ -d "/opt/Jetson_status/" ]; then
 	echo "find old Version"
        sleep 2
	sudo cp -rpf Jetson_status/ /opt/
        echo "reinstall Jetson_status Tool"
        sleep 1
else
	echo "install Jetson_status Tool"
    	sleep 1
    	sudo cp -rpf Jetson_status/ /opt/

fi


if [ -f "/usr/bin/Jetson_status" ]; then
	echo "prepare run,Please Key in [Jetson_status [parameter]]"

else

        sleep 1
    	echo "finish install,Please key in [Jetson_status [parameter]]"

fi
echo "export PATH=$PATH:/opt/Jetson_status" >> ~/.bashrc
echo "system will reboot in 5 sec."
sleep 5

sudo reboot now
