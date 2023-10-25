# TCP Server

TCP server that receives a stream over a connection and write the received data into a output file.

Tools used:

* C++11
* Boost 1.8.0
* CMake 3.25
* Docker

## How to configure

You can set some variables using the `config.json` to run the server such as:

| Config Key | Description |
|---|---|
| server_port | TCP port that the server will listen |
| file_name_prefix | String to prepend on all generated files. The generated files will always have the prefix, the client connection information and a timestamp in local time |
| max_file_size | Max file size in bytes. If the stream received is greater than the `max_file_size`, the output will be cropped. |                                             |                                           |

## How to run locally

If you are on a linux and already have C++ and the Boost Library configured, just compile by running the `build.sh` script on the project folder:

```sh
chmod +x build.sh
./build.sh
```

To run the server:

```sh
build/bin/tcp_server
```

To run the client:

```sh
build/bin/tcp_client <server address> <port>
```

## How to run on docker

To use the docker container, first you'll have to build the image:

```sh
docker build -t tcp_server .
```

Then you can run with `docker compose` command:

```sh
docker compose up
```

To run the client you can use:

```sh
docker exec -it <container_name> tcp_client <host> <port>
```
