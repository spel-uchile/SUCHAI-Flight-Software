#dependencies
sudo apt-get update
sudo apt-get install build-essential
sudo apt-get install python3-pip python-pip
sudo apt-get install git-all
sudo apt-get install libsqlite3-dev
sudo apt-get install libzmq3-dev|
sudo apt-get install cmake
sudo pip install pyzmq-static
sudo apt install python3-gpiozero python-gpiozero
#sudo pip3 install pyzmq-static
sudo apt-get install python3-zmq
#flight software repo
git clone https://github.com/spel-uchile/SUCHAI-Flight-Software.git
#install libcsp dep
cd SUCHAI-Flight-Software
cd src/drivers/Linux/libcsp
sh install_csp.sh
cd /home/pi/Spel
#balloon deps
sudo apt-get install gpsd gpsd-clients python-gps python3-gps
sudo apt-get install python-smbus python3-smbus
sudo apt-get install python-serial python3-serial

#balloon repo
git clone https://github.com/spel-uchile/balloon_experiment.git
ln -s balloon_experiment/ submodules/

#bmp deps
cd ~
git clone https://github.com/adafruit/Adafruit_Python_BMP.git
cd Adafruit_Python_BMP
sudo python setup.py install
sudo python3 setup.py install
