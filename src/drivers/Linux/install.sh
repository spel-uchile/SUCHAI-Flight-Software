#!/usr/bin/env bash

# Download and install LibCSP
cd libcsp
echo "Installing LibCSP..."
./install_csp.sh

# Dowload linenoise
echo "Installing linenoise"
if [ ! -d "./linenoise" ]; then
  git clone https://github.com/antirez/linenoise.git
fi
