#!/bin/bash
set -x

echo "Do you wish to remove default opencv-2.4 (type a number 1 or 2)?"
select yn in "Yes" "No"; do
    case $yn in
        Yes ) 
		sudo apt purge libopencv*
		sudo apt purge opencv*

             	break;;

        No ) 	break;;
    esac
done

sudo apt update
sudo apt upgrade
sudo apt autoremove
sudo ldconfig

