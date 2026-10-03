## Ideas and Future Work

1. **MSHRs in the RTL.** Cache is currently blocking, so cross-validation uses only one MSHR and all memory level parallelism results come from the model only. Integrating MSHR file to the RTL would let the model's non-blocking timing be checked against the hardware.
2. **Multiple Replacement Policies.** Currently, we adopt an LRU only approach, but adding FIFO and random replacement would allow us to compare the efficacy of different policies.
3. **Assertions and Constrained Random Testing** There is further scope for adding assertions, and to run large randomised bundle streams against them. The RTL currently checks one-hot hit detection, hit/miss accounting and misses = memory requests. Further assertions would cover:
- *Coalescer:* every transaction serves at least one lane, only pending lanes, always includes the leader (which is selected to be the lowest pending lane).
- *Cache:* requests accepted only in IDLE, tags written only in FILL, and FILL lasts only one cycle.
- *Replacement:* if any way is empty, the victim is an empty way.

These could then be tested with large randomised bundles (100,000+) rather than only the fixed workloads.
