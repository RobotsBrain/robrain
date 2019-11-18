The http server base on libevent.

git clone the code, then goto source code dir to compile


run the server like this
```
$ ./out/server/test/TestHlsServer --port 8080 --dir /home --host 127.0.0.1
```

you can put "127.0.0.1:8080" in the web browser to test the server

create a hls server
```
./out/server/test/TestHlsServer --port 8080 --dir ./resource/web/ --host 127.0.0.1
```

play hls stream
```
$ ffplay http://127.0.0.1:8080/playlist.m3u8
```