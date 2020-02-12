# AF_LOCAL stress test

Final project for passing the course Programming in Linux

Launch:

massivereader parameters: 
1. server port AF_LOCAL : int number in the range 1024-65535
2. Parameter -O prefix

multiwriter parameters:
1. Parameter -d : int number of connections AF_LOCAL
2. Parameter -p : server port AF_LOCAL - int number in the range 1024-65535
3. Parameter -d : minimal message sending interval in microseconds
4. Parameter -T : total time work in centiseconds