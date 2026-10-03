import json
import sys


workloads = ["broadcast", "coalesced_aligned", "coalesced_misaligned", "strided_k", "reverse", 
             "transpose", "tiled", "random_uniform", "gather", "hotspot"]


counters = ["requests","hits","misses", "evictions", "mem_reqs", "mem_bytes"]

folder = sys.argv[1] if len(sys.argv) > 1 else "results/xval"

failed = 0


for w in workloads:
    with open(folder + "/cpp/" + w + ".json") as f:
        cpp = json.load(f)
    with open(folder + "/rtl/" + w + ".json") as f:
        rtl = json.load(f)

    ok = True

    for c in counters:
        if cpp["counters"][c] != rtl["counters"][c]:
            print(w + ": " + c + " mismatch, cpp=" + str(cpp["counters"][c]) + " rtl=" + str(rtl["counters"][c]))
            ok = False

    if len(cpp["fill_logs"]) != len(rtl["fill_logs"]):
        print(w+ ": number of fills mismatch")
        ok = False
    else:
        for i in range(len(cpp["fill_logs"])):
            if cpp["fill_logs"][i] != rtl["fill_logs"][i]:
                print(w + ": fill " + str(i) + " mismatch, cpp=" + str(cpp["fill_logs"][i]) + " rtl=" + str(rtl["fill_logs"][i]))
                ok = False
                break

    if ok:
        print(w + ": PASS")
    else:
        failed += 1


print(str(len(workloads) - failed) + "/" + str(len(workloads))+ " workloads match")

if failed>0:
    sys.exit(1)