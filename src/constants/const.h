enum Command {
    // string commands
    COMM_SET,
    COMM_GET,

    // generics commands
    COMM_DEL,
    COMM_TYPE,
    COMM_EXPIRE,
    COMM_PERSIST,

    // list commands
    COMM_LPUSH,
    COMM_RPUSH,
    COMM_LPUSHIDX,
    COMM_LPOP,
    COMM_RPOP,
    COMM_LPOPIDX,
    COMM_LLEN,
    COMM_LINDEX,
    COMM_LRANGE,

    // hash commands
    COMM_HSET,
    COMM_HGET,
    COMM_HGETALL,
    COMM_HKEYS,
    COMM_HDEL,
    COMM_HEXISTS,
    COMM_HLEN,
    COMM_HSTRLEN
};