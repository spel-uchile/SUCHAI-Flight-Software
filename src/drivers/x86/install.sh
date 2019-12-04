#!/bin/sh
cd libcsp
sh install_csp.sh
cd -

# Dowload linenoise
echo "Installing linenoise"
if [ ! -d "./linenoise" ]; then
  git clone https://github.com/antirez/linenoise.git
fi