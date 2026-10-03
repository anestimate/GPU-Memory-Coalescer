# Design Specification

The RTL (`rtl/`) and the C++ model (`model/`) implement this specification separately. They are then cross-validated to ensure they match exactly on all functional parameters.


## Parameters (baseline)

32 lanes, 32-bit addresses, 4 byte accesses, 64 B lines, 8 KB cache, 4 ways (32 Sets), 100-cycle latency for memory, 1 MSHR (C++ model only).

Ways, line size and cache size are build parameters (`make xval WAYS=... LINE_BYTES=... CACHE_BYTES =...`) can be passed to both implementations.


## Coalescer

- One transaction per distinct cache line among the active lanes.
- Each transaction is led by the lowest-numbered lane still pending and covers every pending lane on that line; transactions are emitted in leader order.


## Cache

- On a miss, fill the lowest numbered invalid way; if none, evict the LRU way.
- An eviction is counted only when a valid line is replaced.
- LRU is updated on hits and fills.

## Blocking and MSHRs

The RTL is blocking. The model with 1 MSHR is made equivalent (each miss completes before the next access). Larger MSHR counts are model-only: accesses to a line already in flight are secondary misses and fetch nothing new.

## Timing model

Hit = 1-cycle; miss = memory latency + 5; +1 per transaction. Not yet modelled is 1 cycle per bundle and 2 reset cycles.