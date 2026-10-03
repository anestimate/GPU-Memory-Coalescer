# Cross-validation Log


## D1 - Model functional counter mismatch  (In-flight lines counted as misses)

Symptom: After raising the model's mem_latency from 0 to 100 to match the RTL, the functional counters diverged. Hits and misses differed, but mem_reqs and evictions did not. 

First Divergence: Broadcast, access 2. Access 1 misses on line 0 and allocates to MSHR. Access 2 is to the same line while it is still in flight. Model however, still records a secondary miss, the RTL stalls until the fill then hits.

Which side is right: The RTL in this case. DESIGN.md states that the RTL is blocking, and that the exact comparison uses NUM_MSHR = 1. A blocking cache cannot service a second access until the first miss has filled. the Model with NUM_MSHR = 1 still allowed hits and merges while a miss was outstanding - non-blocking with one entry.

Why it was hidden before: with mem_latency = 0 every miss completed before the next access, so the model was blocking by accident.


Fix: Retire the miss immediately when num_mshr = 1. This makes it so that the fill happens before the next access.


Result: 10/10 workloads match on all six counters and fill order.


## D2 - Cycle Counter mismatch

Symptom: The number of cycles for a request is not the same for the RTL and C++ model.


Workload|  Misses | Hits | Bundles | RTL Cycles | Model Cycles | RTL - Model | Error
|---|---|---|---|---|---|---|---|
Broadcast           | 1       | 99   | 100     | 406        | 299          | 107         | -26.4%
Coalesced_aligned   | 200     | 0    | 100     | 21302      | 20200        | 1102        | -5.2%
Coalesced_misaligned | 201    | 99   | 100     | 21606      | 20499        | 1107        | -5.1%
Strided_k           | 800     | 0    | 100     | 84902      | 80800        | 4102        | -4.8%
Reverse             | 200     | 0    | 100     | 21302      | 20200        | 1102        | -5.2%
Transpose           | 65536   | 0    | 2048    | 6948866    | 6619136      | 329730      | -4.7%
Tiled               | 4096    | 0    | 2048    | 436226     | 413696       | 22530       | -5.2%
Random_uniform      | 22      | 1676 | 100     | 5786       | 5574         | 212         | -3.7%
Gather              | 36      | 1380 | 100     | 6678       | 6396         | 282         | -4.2%
Hotspot             | 36      | 1251 | 100     | 6420       | 6138         | 282         | -4.4%


Finding: on every workload, RTL-Model = 5*(misses) + 1*(bundles) +2.

Why this mismatch occurs: Each simulator counts time differently. For example, a miss causes 101 cycles in the C++ model, whereas the RTL takes 106 cycles. The rst() function in the RTL calls tick() twice and this adds to total_cycles. When a miss occurs, the RTL miss path passes through FSM states which the model doesn't represent, which causes an additional 5 cycles to be added.

Which side is right: The RTL represents ground truth, but model abstracts the FSM miss path and the coalescer handshake.

Fix: Model the missing mechanisms, which should fix the mismatch. The 5 cycle miss-path was added to the model (reducing MAPE from 6.9% to 3.2%). The 1 cycle/bundle coalescer handshake and 2 reset cycles are left unmodelled; see Timing Correlation.


## D3 - HIT_WAY Error

Symptom: RTL failed to build at WAYS=1.

Finding: The hit_way -> access_way decode was a case statement with 4 ways. At ways =1, the widths do not fit causing a build error. At WAYS=8, it would build but it would send the hits in ways 4-7 to way 0, giving incorrect LRU updates silently.

Which side was wrong: RTL.

Fix: Replaced the hardcoding with a for-loop that works with variable inputs.


## D4 - OPT Error

Symptom: In some instances, the misses recorded by OPT exceeded that of the LRU. 

Finding: The OPT was matching exact addresses instead of lines. It was caught by the sweep feature. Unit tests missed this because their addresses were multiples of 64.

Which side was wrong: C++ model

Fix: Compared line addresses in get_next_use.

## D5 - Assertions Error

Symptom: The SVA properties in assertions.svh had never fired, even though one of them was logically wrong.

Finding: SVA silenty disabled (no `--assert` put in makefile) 

Fix: Added --assert to the build and corrected the accounting property, and this resulted in 0 failures across 54.6M cycles.

## Result - Functional Equivalence Grid


Tested 8 different configs for 10 different workloads each, resulting in 80 runs. All exact match on six counters and fill order. In total, 610,035 cache requests and 45,664 fills compared (fill logs were capped at 2048 per run).


Config| Ways | Line | Cache | Result |
|---|---|---|---|---|
w4_l64_c8192 (baseline) | 4 | 64| 8 KB| 10/10| 
w1_l64_c8192 | 1 | 64 | 8 KB | 10/10 | 
w2_l64_c8192 | 2 | 64 | 8 KB | 10/10 | 
w8_l64_c8192 | 8 | 64 | 8 KB | 10/10 | 
w4_l32_c8192 | 4 | 32 | 8 KB | 10/10 | 
w4_l128_c8192| 4 | 128| 8 KB | 10/10 | 
w4_l64_c2048 | 4 | 64 | 2 KB | 10/10 | 
w4_l64_c32768| 4 | 64 | 32 KB| 10/10 | 


# Result - Timing Correlation

Correlation results:

n = 80 paired runs
MAPE     6.91%   (target<10%)
Spearman rho  1.0   (target>0.95)
signed mean  -6.91%   (negative = model too low)
max error     -26.35%   (w1_l64_c8192 broadcast)

After adding +5 clock increase to model after every miss, the following were the results:

n = 80 paired runs
MAPE     3.21%   (target<10%)
Spearman rho  1.0   (target>0.95)
signed mean  -3.21%   (negative = model too low)
max error     -25.12%   (w1_l64_c8192 broadcast)

This is consistent with the output of the formula beforehand.

The errors still left are the 2 reset cycles in the RTL and 1 cycle per bundle due to the coalescer accepting the bundle, which are both left unmodeled on purpose.


## Parameter Sweep Timing

E1_baseline: done
E2_line_size: done
E3_ways: done
E4_mshrs: done
E5_capacity: done
E6_alignment: done
E7_locality: done
257 runs across 7 experiments in 1.1 seconds.

The results above are available in results/sweeps/.


Each model run takes around 4 ms, likely dominated by process start-up. No direct RTL vs model speed figure is calculated in this instance, because the RTL runs have tracing enabled and the workloads are too small for a fair comparison.


## Results Analysis [Sweep]

E1_baseline: The same 32 lane load costs very different amounts depending on the access pattern. Transactions per bundle range from 1 to 32. 

Transpose vs tiled is the clearest example: both read the same 256x256 matrix but transpose walks column by column, while tiled walks it in 16x16 tiles. Tiled therefore needs 16x fewer transactions and runs 16x faster.

E2_line_size: for coalesced_aligned, transactions per bundle drop from 4 at 32 B to 1 at 256 B. This is because bigger lines cover more lanes. For transpose it stays at 32 at every size, because each lane is on a different line however big the lines are.

E3_ways: Associativity only matters for transpose. For the other 9 workloads, misses and cycles are identical at every way count because they either stream through new lines without reuse or already fit in the cache, so there are no conflicts to remove. Transpose misses on every access up to 16 ways (65536 misses), but at 32 ways this drops to just 4096, and thus cycles fall from around 6.9 million to 557,000, because less waiting is needed for the lines to arrive from main memory.

E4_mshrs: An increased number of MSHRs let misses overlap, so cycles roughly halve each time MSHRs double. However, this saturates at about 16 for most workloads (16 and 32 give identical cycles) because of Little's law. The number of misses in flight is roughly equal to memory latency/time between misses. It takes around 100 cycles for a miss to return, and a new one can only be issued ~7.5 cycles, so at most 13 can be in flight. 16 MSHRs already covers that, so increasing to 32 has little effect. The miss count rises with more MSHRs because accesses to a line already in flight are counted as secondary misses.

E5_capacity: For transpose, cache_bytes has a significant effect. The number of misses drops from 65536 to 4096 when the cache bytes goes from 16 KB to 32 KB. This is a set-conflict effect. Transpose's 32 lines only take 32*64 = 2048 bytes but with 4 ways the number of sets is cache bytes/64/4, so 16 KB (64 sets) the 32 lines fall into only 4 sets with 8 lines each, more than the 4 ways, so they evict each other; at 32 KB (128 sets) they fall into 8 sets with 4 lines each, which fits. 

Hotspot and Gather also change; gather goes from 145 misses to 36, and hotspot goes from 111 misses to 36 between 2 KB and 4KB.

E6_alignment: The transactions per bundle changes from 2 to 3 when the offset goes from 0 to 4 and above. This is because 128 bytes of data can only fit between a maximum of 3 lines, so a maximum of 3 transactions are needed to extract that data.

E7_locality: The locality setting controls how close together the lanes from 'gather' are: each lane picks from a small 400 byte (7 lines) with probability = locality and from a larger ~2.3 KB region (36 lines) otherwise.

As locality rises from 0 to 1, transactions per bundle fall steadily from 21.5 to 6.74, because more lanes land in the same few lines and the coalescer merges them. Misses however, stay at 36 at every locality below 1. This is because the whole 2.3 KB region fits in the 8 KB cache, so each of its 36 lines misses only once (the first time it's touched). At locality of 1, where every lane stays in the small region, misses drops to 7.



LRU vs OPT: LRU matches OPT whenever the data fits, and LRU falls behind when the cache is just too small or sets are overcrowded. Note that OPT is not possible to implement in real hardware since it requires knowledge of future accesses.