cmake -S . -B build-asan

cmake --build build-asan --target April

Terminal 1:
./build-asan/system_server
Server listening on 0.0.0.0:50051

Terminal 2:
./build-asan/system_client
CPU: 42%
MEM: 65%
TEMP: 55.5
MODE: eco
Success: 1
Mode: performance
Msg: Mode updated

