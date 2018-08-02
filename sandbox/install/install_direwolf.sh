#install direwolf
#install subdep sound:
#sudo apt-get install libasound2-dev
#sudo apt-get install libgps-dev
cd ~
git clone https://github.com/wb2osz/direwolf.git
cd direwolf
make
sudo make install
make install-conf
make install-rpi
