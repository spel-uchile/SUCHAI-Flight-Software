#/bin/env bash
git clone -b simulator git@github.com:spel-uchile/SUCHAI-Flight-Software.git tmp
mv tmp/* tmp/.[^.]* .
rm -r ./tmp
build_cmd="LINUX SIMULATOR --name SIMULATOR --id 1 --hk 1 --com 1 --fp 0 --sen 0 --con 0 --node 1 --st_mode 0 --drivers"
echo "Configure (and build) SUCHAI-FS using:"
echo $build_cmd
python3 compile.py $build_cmd
