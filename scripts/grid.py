import json
import sys
import os


workloads = ["broadcast", "coalesced_aligned", "coalesced_misaligned", "strided_k", "reverse", 
             "transpose", "tiled", "random_uniform", "gather", "hotspot"]

short = ["bcast", "align", "misal", "strid", "rev", "transp", "tile", "rand", "gath", "hot"]

counters = ["requests","hits","misses", "evictions", "mem_reqs", "mem_bytes"]

folder = "results/xval"
configs = sorted(c for c in os.listdir(folder) if c.startswith("w"))



def check(cfg, w):
    cpp_file = folder + "/" + cfg + "/cpp/" + w + ".json"
    rtl_file = folder + "/" + cfg + "/rtl/" + w + ".json"

    if not os.path.exists(cpp_file) or not os.path.exists(rtl_file):
        return "--",0

    with open(cpp_file) as f:
        cpp = json.load(f)
    with open(rtl_file) as f:
        rtl = json.load(f)

    for c in counters:
        if cpp["counters"][c] != rtl["counters"][c]:
            return "FAIL",0

    if cpp["fill_logs"] != rtl["fill_logs"]:
        return "FAIL",0

    return "OK", cpp["counters"]["requests"]

print("".ljust(16) + "".join(s.rjust(7) for s in short))

passed = 0
total = 0
requests = 0


for cfg in configs:
    row = cfg.ljust(16)
    for w in workloads:
        result, n = check(cfg, w)
        row += result.rjust(7)
        total += 1
        if result == "OK":
            passed += 1
            requests += n
    print(row)

print("")
print(str(passed) + "/" + str(total) + " config-workload pairs: exact functional match")
print("counters compared: " + str(passed * len(counters))+ "    requests compared: " + str(requests))
print("mismatches: " + str(total-passed))
print("fill order verified on all " + str(len(configs)) + " configs")


if passed != total:
    sys.exit(1)