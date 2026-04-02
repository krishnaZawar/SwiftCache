#include<string>

#ifndef errors_header
#define errors_header

using std::string;

const string ERR_KEY_NOT_FOUND = "Error: Key not found";
const string ERR_TYPE_MISMATCH = "Error: key type mismatch";
const string ERR_EXPECTED_INTEGER = "Error: Expected an integer value";
const string ERR_NO_COMM = "Error: Invalid command";

// string commands
const string ERR_SET_COMM = "Error: Invalid usage of SET command";
const string ERR_GET_COMM = "Error: Invalid usage of GET command";

// generic commands
const string ERR_DEL_COMM = "Error: Invalid usage of DEL command";
const string ERR_EXIT_COMM = "Error: Invalid usage of EXIT command";
const string ERR_TYPE_COMM = "Error: Invalid usage of TYPE command";
const string ERR_EXPIRE_COMM = "Error: Invalid usage of EXPIRE command";
const string ERR_PERSIST_COMM = "Error: Invalid usage of PERSIST command";

// list commands
const string ERR_INDEX_OUT_OF_BOUNDS = "Error: Index out of bounds";
const string ERR_POP_COUNT_UNDERFLOW = "Error: Pop count cannot be negative";
const string ERR_POP_COUNT_OVERFLOW = "Error: pop count is greater than list size";
const string ERR_LPUSH_COMM = "Error: Invalid usage of LPUSH command";
const string ERR_RPUSH_COMM = "Error: Invalid usage of RPUSH command";
const string ERR_LPUSHIDX_COMM = "Error: Invalid usage of LPUSHIDX command";
const string ERR_LPOP_COMM = "Error: Invalid usage of LPOP command";
const string ERR_RPOP_COMM = "Error: Invalid usage of RPOP command";
const string ERR_LPOPIDX_COMM = "Error: Invalid usage of LPOPIDX command";
const string ERR_LLEN_COMM = "Error: Invalid usage of LLEN command";
const string ERR_LINDEX_COMM = "Error: Invalid usage of LINDEX command";
const string ERR_LRANGE_COMM = "Error: Invalid usage of LRANGE command";

// hash commands
const string ERR_HSET_COMM = "Error: Invalid usage of HSET command";
const string ERR_HGET_COMM = "Error: Invalid usage of HGET command";
const string ERR_HGETALL_COMM = "Error: Invalid usage of HGETALL command";
const string ERR_HKEYS_COMM = "Error: Invalid usage of HKEYS command";
const string ERR_HDEL_COMM = "Error: Invalid usage of HDEL command";
const string ERR_HEXISTS_COMM = "Error: Invalid usage of HEXISTS command";
const string ERR_HLEN_COMM = "Error: Invalid usage of HLEN command";
const string ERR_HSTRLEN_COMM = "Error: Invalid usage of HSTRLEN command";
const string ERR_HASH_FIELD_DOES_NOT_EXIST = "Error: field does not exist";

#endif // errors_header