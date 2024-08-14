cd $(dirname $0)
make clean
make
sudo ./aesdchar_unload
sudo ./aesdchar_load
