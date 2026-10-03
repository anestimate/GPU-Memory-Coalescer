
import json
import os
import math

folder = "results/xval"
configs = sorted(c for c in os.listdir(folder) if c.startswith("w"))

workloads = ["broadcast", "coalesced_aligned", "coalesced_misaligned", "strided_k", "reverse", 
             "transpose", "tiled", "random_uniform", "gather", "hotspot"]

rtl_cycles = []
model_cycles = []
names = []


csv = open("results/cycles_after.csv", "w")
csv.write("config,workload,rtl_cycles,model_cycles\n")

for cfg in configs:
    for w in workloads:
        with open(folder + "/" + cfg + "/cpp/" + w + ".json") as f:
            cpp = json.load(f)
        with open(folder + "/" + cfg + "/rtl/" + w + ".json") as f:
            rtl = json.load(f)


        r = rtl["counters"]["cycles"]
        m = cpp["counters"]["cycles"]
        rtl_cycles.append(r)
        model_cycles.append(m)
        names.append(cfg + " " + w)
        csv.write(cfg+ "," + w + "," + str(r) + "," + str(m) + "\n")


csv.close()

def ranks(values):
    s = sorted(values)
    result = []
    for v in values:
        first = s.index(v)+1
        last = first + s.count(v) -1
        result.append((first+last)/2)
    return result

def correlation(a,b):
    mean_a = sum(a)/len(a)
    mean_b = sum(b)/len(b)
    cov = sum((x-mean_a)*(y-mean_b) for x,y in zip(a,b))
    var_a = sum((x-mean_a)**2 for x in a)
    var_b = sum((y-mean_b)**2 for y in b)
    return cov/math.sqrt(var_a*var_b)


errors = []
for r,m in zip(rtl_cycles, model_cycles):
    errors.append((m-r)/r *100)

mape = sum(abs(e) for e in errors) / len(errors)
signed = sum(errors)/len(errors)
rho = correlation(ranks(rtl_cycles), ranks(model_cycles))

worst = max(range(len(errors)), key=lambda i: abs(errors[i]))

print("n = " + str(len(errors)) + " paired runs")
print("MAPE     " + str(round(mape,2)) + "%   (target<10%)")
print("Spearman rho  " + str(round(rho,4)) + "   (target>0.95)")
print("signed mean  " + str(round(signed,2)) + "%   (negative = model too low)")
print("max error     " + str(round(errors[worst],2)) + "%   (" +names[worst] + ")")

