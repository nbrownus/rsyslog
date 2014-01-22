#!/bin/sh

# Get the ubuntu key
sudo apt-key adv --recv-keys --keyserver keyserver.ubuntu.com AEF0CF8E

# Add the repo
#TODO make this match the branch
sudo add-apt-repository -y "deb http://ubuntu.adiscon.com/v7-stable precise/"

sudo apt-get update
sudo apt-get install liberstr