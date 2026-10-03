
import json
import os
import subprocess
import time

workload = ["broadcast", "coalesced_aligned", "coalesced_misaligned", "strided_k", "reverse",
            "transpose", "tiled", "random_uniform", "gather", "hotspot"]


experiments = [
    ("E1_baseline", "ways", [4], workload),
    ("E2_line_size", "line_bytes", [32,64,128,256], workload),
    ("E3_ways", "ways", [1,2,4,8,16,32,128], workload),
    ("E4_mshrs", "num_mshr", [1,2,4,8,16,32], workload),
    ("E5_capacity", "cache_bytes", [2048,4096,8192,16384,32768], workload),
    ("E6_alignment", "offset", list(range(0,64,4)), ["coalesced_misaligned"]),
    ("E7_locality", "locality", [i/10 for i in range(11)], ["gather"]),

]

os.makedirs("results/sweeps",exist_ok=True)
start = time.time()
runs = 0

for name, param, values, wls in experiments:
    csv = open("results/sweeps/" + name + ".csv", "w")
    csv.write("workload," + param + ",transactions_per_bundle,hit_rate,misses,opt_misses,mem_bytes,cycles\n")

    for w in wls:
        for v in values:
            out = subprocess.run(["./model/model",w,param+"=" + str(v)],capture_output=True,text=True)
            if out.returncode != 0:
                print("Failed: " + name + " " + w + " " + param + "=" + str(v))
                continue

            d = json.loads(out.stdout)
            c = d["counters"]
            tpb = c["transactions"]/c["bundles"]
            hit_rate = c["hits"]/c["requests"]

            csv.write(w + "," + str(v) + "," + str(tpb) + "," + str(hit_rate) + "," + str(c["misses"])+ ","
                      +str(c["opt_misses"]) + "," + str(c["mem_bytes"]) +  "," + str(c["cycles"]) + "\n")
            runs +=1

    csv.close()
    print(name + ": done")

print(str(runs) + " runs in " + str(round(time.time() - start,1))+ " seconds")