#!/bin/sh
cd libcsp
sh install_csp.sh
cd -

# Download linenoise
echo "Installing linenoise..."
if [ ! -d "./linenoise" ]; then
  git clone https://github.com/antirez/linenoise.git
fi

# Download sgp4
echo "Installing SGP4..."
if [ ! -d "./sgp4" ]; then
  git clone https://github.com/spel-uchile/sgp4.git
fi
