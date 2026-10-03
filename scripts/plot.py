import csv
import os
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

plt.rcParams.update({
    "font.family": "serif",
    "font.size": 10,
    "axes.labelsize": 10,
    "xtick.labelsize":9,
    "ytick.labelsize":9,
    "legend.fontsize":9,
    "legend.frameon": False,
    "axes.spines.top": False,
    "axes.spines.right": False,
    "axes.grid": True,
    "grid.alpha": 0.3,
    "grid.linestyle": "--",
    "savefig.bbox": "tight",

})

NAMES = {
    "broadcast": "Broadcast",
    "coalesced_aligned": "Aligned",
    "coalesced_misaligned": "Misaligned",
    "strided_k": "Strided",
    "transpose": "Transpose",
    "reverse": "Reverse",
    "tiled": "Tiled",
    "random_uniform": "Random",
    "gather": "Gather",
    "hotspot": "Hotspot",
}

SWEEPS = "results/sweeps/"
OUT = "docs/figures/"


def read_csv(path):
    with open(path) as f:
        return list(csv.DictReader(f))

def save(fig, name):
    fig.savefig(OUT + name + ".pdf")
    fig.savefig(OUT + name + ".png", dpi =200)
    plt.close(fig)



def fig1_pattern_cost():
    rows = read_csv(SWEEPS + "E1_baseline.csv")
    rows.sort(key=lambda r:float(r["transactions_per_bundle"]))

    names = [NAMES[r["workload"]] for r in rows]
    values = [float(r["transactions_per_bundle"]) for r in rows]

    fig, ax = plt.subplots(figsize=(5,3))
    ax.barh(names,values)
    ax.axvline(1, linestyle="--", color="grey")
    ax.axvline(32,linestyle="--", color="grey")
    ax.set_xlabel("Transactions per Bundle")
    fig.tight_layout()
    save(fig , "fig1_pattern_cost")


def fig2_transpose_tiled():
    rows = read_csv(SWEEPS + "E1_baseline.csv")
    t = [r for r in rows if r["workload"] == "transpose"][0]
    s = [r for r in rows if r["workload"] == "tiled"][0]

    metrics = [("transactions_per_bundle", "Transactions per Bundle"),
               ("misses", "Misses"), ("mem_bytes", "Memory Traffic (Bytes)"), 
               ("cycles", "Clock Cycles")]

    fig, axes = plt.subplots(1,4,figsize=(10,2.8))

    for i, (key,label) in enumerate(metrics):
        axes[i].bar(["Transpose", "Tiled"], [float(t[key]), float(s[key])], color=["#D55E00","#0072B2"])
        axes[i].set_title(label)
        axes[i].grid(axis="x",visible=False)

    save(fig , "fig2_transpose_v_tiled")

def fig3_LRU_OPT():
    rows = read_csv(SWEEPS + "E5_capacity.csv")

    fig, axes = plt.subplots(1,3, figsize=(10,3))

    for ax,w in zip(axes,["transpose", "gather", "hotspot"]):

        wr = [r for r in rows if r["workload"] == w]
        sizes = [float(r["cache_bytes"])/1024 for r in wr]
        lru = [float(r["misses"]) for r in wr]
        opt = [float(r["opt_misses"]) for r in wr]
        line = ax.plot(sizes, lru, marker = "o", label ="LRU")
        ax.plot(sizes, opt, marker = "o",linestyle = "--", color=line[0].get_color(),label="OPT")
        ax.set_title(NAMES[w])
        ax.set_xscale("log",base=2)
        ax.set_xticks(sizes)
        ax.set_xticklabels([f"{s:g}" for s in sizes])
        ax.set_xlabel("Cache Size (KB)")



    axes[0].set_ylabel("Misses")
    axes[0].legend()
    save(fig, "fig3_LRU_OPT")

def fig4_mshr():
    rows = read_csv(SWEEPS + "E4_mshrs.csv")

    fig, ax= plt.subplots(figsize=(5,3.5))

    for w in ["coalesced_aligned", "strided_k", "transpose", "gather"]:

        wr = [r for r in rows if r["workload"] == w]
        mshrs = [float(r["num_mshr"]) for r in wr]
        cycles = [float(r["cycles"]) for r in wr]
        ax.plot(mshrs, cycles, marker = "o",label=NAMES[w])

    ax.axvline(13, linestyle="--",color="grey", label="Little's Law Estimate (~13)")
    ax.set_xlabel("Number of MSHRs (log scale)")
    ax.set_ylabel("Cycles (log scale)")
    ax.set_yscale("log")
    ax.set_xscale("log", base=2)
    ax.legend()
    save(fig , "fig4_mshr")


def fig5_alignment():
    rows = read_csv(SWEEPS + "E6_alignment.csv")
    offsets = [float(r["offset"]) for r in rows]
    tbp = [float(r["transactions_per_bundle"]) for r in rows]

    fig, ax = plt.subplots(figsize=(5,3))
    ax.plot(offsets,tbp,marker="o")
    ax.set_ylim(0,4)
    ax.set_xticks(range(0,64,8))
    ax.set_xlabel("Address Offset (bytes)")
    ax.set_ylabel("Transactions per Bundle")

    save(fig,  "fig5_alignment")

def fig6_model_vs_rtl():
    
    fig, ax= plt.subplots(figsize=(4.5,4))

    for path, label in [("results/cycles_before.csv", "Before Miss Overhead"), 
                        ("results/cycles_after.csv", "After Miss Overhead")]:

        rows = read_csv(path)
        rtl = [float(r["rtl_cycles"]) for r in rows]
        model = [float(r["model_cycles"]) for r in rows]
        errors = [abs(m-r)/r *100 for r, m in zip(rtl,model)]
        mape = sum(errors)/len(errors)
        ax.scatter(rtl, model, s=15, label=label + f" (MAPE {mape:.1f}%)")

    low, high = min(rtl), max(rtl)
    ax.plot([low,high], [low,high], color="black", linewidth=0.8, label="y = x")
    ax.set_xlabel("RTL Cycles (log scale)")
    ax.set_ylabel("Model Cycles (log scale)")
    ax.set_yscale("log")
    ax.set_xscale("log")
    ax.legend()
    save(fig , "fig6_rtl_model")


os.makedirs(OUT, exist_ok=True)
fig1_pattern_cost()
fig2_transpose_tiled()
fig3_LRU_OPT()
fig4_mshr()
fig5_alignment()
fig6_model_vs_rtl()