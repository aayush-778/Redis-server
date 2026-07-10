#!/usr/bin/env bash
set -e

# Filter out comment lines, then pipe into redis-cli:
sed '/^#/d' << 'EOF' | redis-cli -p 6379
# 1) Start clean
FLUSHALL

# 2) Common Commands
PING
ECHO Hello

# 3) Key/Value Operations
SET city Berlin
GET city
KEYS *
TYPE city
DEL city
SET session1 foo
EXPIRE session1 1
GET session1         # after ~1s should return (nil)
RENAME session1 session2
GET session2

# 4) List Operations
RPUSH colors red yellow green
LGET colors
LLEN colors
LPUSH colors start
RPUSH colors end
LPOP colors
RPOP colors
RPUSH colors x y x z x
LREM colors 2 x
LREM colors 0 x
LINDEX colors 1
LINDEX colors -1
LSET colors 1 white
LGET colors

# 5) Hash Operations
HSET user:1 name Alice age 30 email alice@example.com
HGET user:1 email
HEXISTS user:1 address
HEXISTS user:1 name
HDEL user:1 age
HEXISTS user:1 age
HLEN user:1
HKEYS user:1
HVALS user:1
HGETALL user:1
HMSET user:2 name Bob age 25 city Paris
HGETALL user:2
EOF