#! /bin/sh



INFILE=$1
IP=""

get_ip_address()
{
	os=`uname`

	case $os in
   	Linux) 
		IP=`ifconfig | grep 'inet addr:'| grep -v '127.0.0.1' | cut -d: -f2 | awk '{ print $1}'`
		;;

   	FreeBSD|OpenBSD) 
		IP=`ifconfig | grep -E 'inet.[0-9]' | grep -v '127.0.0.1' | awk '{ print $2}'`
		;;

   	SunOS) IP=`ifconfig -a | grep inet | grep -v '127.0.0.1' | awk '{ print $2} '`
		;;

   	*) 
		IP="Unknown"
		;;
	esac
}

build_m3u8_segmenter()
{
	if [ ! -d m3u8-segmenter ]; then
		git clone https://github.com/txgcwm/m3u8-segmenter.git
	fi

	cd m3u8-segmenter
	#aclocal
	#automake -ac
	#./configure
	#make
	gcc -Wall -g  m3u8-segmenter.c -o segmenter -lavformat -lavcodec -lavutil -lm -lz -lpthread

	cp segmenter ../
	cd ..
}

check_ts_file()
{
	extension="${INFILE##*.}"

	if [ x"$extension" = x"ts" ]; then
		TSFILE=${INFILE}
	else
		TSFILE="temp.ts"
		ffmpeg -y -i ${INFILE} -vcodec copy -acodec copy -vbsf h264_mp4toannexb ${TSFILE}
	fi
}

generate_m3u8_ts()
{
	#./segmenter -i ${TSFILE} -n 10 -p hls_test -m test.m3u8 -u http://${IP}:8080/web/
	ffmpeg -i ${TSFILE} -c copy -map 0 -f segment -segment_list playlist.m3u8 -segment_time 10 test%02d.ts
}



#build_m3u8_segmenter
#get_ip_address
check_ts_file
generate_m3u8_ts


# ffmpeg -y -i <in file> -vcodec copy -acodec copy -vbsf h264_mp4toannexb <output file>
# ./segmenter -i out.ts -n 10 -p segmenter_test -m test.m3u8 -u http://192.168.1.100:8080/hls/
# ffmpeg -i temp.ts -c copy -map 0 -f segment -segment_list playlist.m3u8 -segment_time 10 test%02d.ts