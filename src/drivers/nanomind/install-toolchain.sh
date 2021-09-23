#!/bin/sh
if [ ! -d "suchai-docker" ]; then
    git clone https://github.com/spel-uchile/suchai-docker.git
else
    cd suchai-docker
    git pull
    cd -
fi

cd suchai-docker/suchai-fs/avr32-toolchain-3.4.2
chmod +x install-avr32.sh
./install-avr32.sh 3.4.3.820 6.1.3.1475 $HOME/.local
cd -
