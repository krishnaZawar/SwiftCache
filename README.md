
# <p align = "center">SwiftCache</p>
The ultimate single-threaded in-memory database written from scratch in C++.
## How to use
1. Build the database using
```
g++ src/main.cpp -o db -lws2_32
```
2. Start the DB server using
```
./db -df <.txt file> -di <interval in seconds>	# for persistence
./db -p <port number>							# to run on specific port
./db											# to run on default 8080
```
Note: These flags can be combined as and when needed.

## type interfaces
### String
```
SET [<key> <value>]+
GET [<key>]+
```
### List
```
LPUSH <key> [<value>]+
RPUSH <key> [<value>]+
LPUSHIDX <key> <value> <idx>
LPOP <keyname> <count>
RPOP <keyname> <count>
LPOPIDX <keyname> <index>
LLEN <keyname>
LINDEX <keyname> [<index>]+
LRANGE <keyname> <start index> <end index>
```
### Hash
```
HSET <keyname> [<fieldname> <value>]+
HGET <keyname> [<fieldname>]+
HGETALL <keyname>
HKEYS <keyname>
HDEL <keyname> [<fieldname>]+
HEXISTS <keyname> [<fieldname>]+
HLEN <keyname>
HSTRLEN <keyname> [<fieldname>]+
```
### Generic functions
```
EXIT
DEL [<keyname>]+
TYPE [<keyname>]+
EXPIRE [<keyname> <time in seconds>]+
PERSIST [<keyname>]+
```
## Features
### Single threaded
The DB works on a single thread to avoid race conditions and locking overheads for increased efficieny. This thread runs the operations, rehashing loop and expiry loop all in one with high performance.

### Fast Lookups
The DB performs amortized **O(1) lookups** to provide low latency.

### Multiple types supported
The DB supports **String, List, Hash datatypes**.

### TTL support
The DB supports TTL, which allows you to set expiration time for keys, after which the keys are evicted from the DB.
The eviction is done via an exipry loop that runs incrementally in small batches to not block the thread and still maintain operational speed of amortized **O(1)**.

### Rehashing
The DB performs rehashing to stabilize the load factor and keep the operations capped at an amortized **O(1)**. 
Rehashing is done incrementally in small batches to not block the thread completely and still be able to deliver low latency. The rehashing also takes care of expiration as it blocks the expiry loop during rehashing.

### Type safety
A key can only hold one type of data at a time. To change the datatype, the key needs to be removed and inserted again into the DB.

### Persistence
The **Write-Ahead Log (WAL)** is used to persist the database state. It enables recovery after crashes and prevents data loss during restarts. By default, persistence is disabled for performance reasons, but it can be enabled by providing a `dumpFile` and `dumpInterval` when starting the server.
## Future scope of development
1. IO multiplexing to serve multiple clients
2. Write a driver to make the DB more integrable and easy to use
3. Eviction mechanism (LRU, LFU, PLRU depending on performance)
