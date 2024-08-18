

mode=$1

cd /lib/modules/$(uname -r)/extra

case $mode in
        start)
        	echo "Loading aesdchar driver"
        	aesdchar_load
        ;;

        stop)
        	echo "Unloading aesdchar driver"
        	aesdchar_unload
        ;;

        *)
        	echo "Usage: $0 {start | stop}"
        exit 1
esac
exit 0
