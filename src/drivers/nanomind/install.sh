#!/bin/sh

TOOLS=${1:-0}  # Install toolchain, default false

if TOOLS; then
  echo "Downloading toolchain..."
  wget -N data.spel.cl/gs-avr32-toolchain-3.4.2.tar.gz
  tar -xzf gs-avr32-toolchain-3.4.2.tar.gz

  echo "Installing toolchain..."
  cd gs-avr32-toolchain-3.4.2/
  if [ ! -d "$HOME/.local" ]; then
      mkdir ~/.local
  fi
  if [ ! -d "$HOME/.local/avr32" ]; then
      mkdir ~/.local/avr32
  fi
  if [ ! -d "$HOME/.local/bin" ]; then
      mkdir ~/.local/bin
  fi
  if [ ! -d "$HOME/.local/bin" ]; then
      mkdir ~/.local/share
  fi
  ./install-avr32.sh
  cd -

  echo "Adding directories to PATH..."
  if echo ":$PATH:" | grep -q ":$HOME/.local/bin:$HOME/.local/avr32/bin:"; then
    echo "Path was correctly set"
  else
    echo 'export PATH="$HOME/.local/bin:$HOME/.local/avr32/bin:$PATH"' >> ~/.bashrc
    source ~/.bashrc
  fi
fi

echo "Checking avr32-gcc Version..."
avr32-gcc --version

echo "Downloading OBC drivers SDK..."
if [ ! -d "suchai-drivers-obc" ]; then
    git clone https://github.com/spel-uchile/suchai-drivers-obc
else
    cd suchai-drivers-obc
    git pull
    cd -
fi

echo "Linking source code into SDK..."
cd suchai-drivers-obc
mv src/ src.old/
ln -s -f ../../../../src
cd -

# echo "Adding libcsp v1.5.dev to sdk lib folder"
# if [ ! -d "libcsp" ]; then
#     git clone https://github.com/libcsp/libcsp
#     cd libcsp
#     git checkout release-1.5
#     echo "Replacing csp drivers..."
#     mv include/csp/drivers/ include/csp/drivers.bak
#     sed -i "s/#include <csp\/drivers\/i2c.h>/#include <dev\/i2c.h>/g" src/interfaces/csp_if_i2c.c
#     sed -i "s/#include <csp\/drivers\/i2c.h>/#include <dev\/i2c.h>/g" include/csp/interfaces/csp_if_i2c.h
#     # I had a bug in arch: using sed the file loss permissions
#     chmod 644 src/interfaces/csp_if_i2c.c
#     chmod 644 include/csp/interfaces/csp_if_i2c.h
#     mv wscript wscript.old
#     cd -
#     cp wscript.libcsp libcsp/wscript
# fi

# echo "Linking libcsp into SDK"
# cd a3200-sdk-lite-v1.2/lib/
# ln -s -f ../../libcsp
# cd -

echo "Downloading BUS drivers..."
if [ ! -d "suchai-drivers-bus" ]; then
    git clone https://github.com/spel-uchile/suchai-drivers-bus suchai-drivers-bus
else
    cd suchai-drivers-bus
    git pull
    cd -
fi

echo "Linking BUS drivers into SDK"
cd suchai-drivers-obc/lib/
for f in ../../suchai-drivers-bus/lib/*; do ln -s -f $f; done
cd -

echo "Preparing build tools"
cd suchai-drivers-obc
python2 ./tools/buildtools/gsbuildtools_bootstrap.py

cd -
#echo "---------"
#echo "Do not forget to add the following to your .bashrc file"
#echo "export PATH=\"\~/.local/bin:\$PATH\""
#echo "export PATH=\"\~/.local/avr32/bin:\$PATH\""

# echo "---------"
# echo "Finished. Compile and program with:"
# echo "sh build.sh"
# echo "sh build.sh program"
# echo "---------"
