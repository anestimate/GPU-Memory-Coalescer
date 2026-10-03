TOP := gpu_mem_top

RTL := \
	rtl/gpu_mem_pkg.sv \
	rtl/tag_array.sv \
	rtl/cache_ctrl.sv \
	rtl/gpu_mem_top.sv \
	rtl/replacement.sv \
	rtl/coalescer.sv \
	rtl/mem_model.sv

TB := tb/tb_top.cpp
SIM := $(OBJ)/V$(TOP)

W ?= broadcast

WAYS ?= 4
LINE_BYTES ?= 64
CACHE_BYTES ?= 8192
CFG := w$(WAYS)_l$(LINE_BYTES)_c$(CACHE_BYTES)


DEFINES := CFG_WAYS=$(WAYS) CFG_LINE_BYTES=$(LINE_BYTES) CFG_CACHE_BYTES=$(CACHE_BYTES)
OBJ := obj_dir/$(CFG)
SIM := $(OBJ)/V$(TOP)
WORKLOADS := broadcast coalesced_aligned coalesced_misaligned strided_k reverse transpose tiled random_uniform gather hotspot
XVAL := results/xval/$(CFG)
MODEL_ARGS := ways=$(WAYS) line_bytes=$(LINE_BYTES) cache_bytes=$(CACHE_BYTES)
VFLAGS := --cc --exe --build -j 0 --trace --assert -Wall -Wno-UNUSEDPARAM -Wno-UNUSEDSIGNAL -y rtl --top-module $(TOP) --Mdir $(OBJ) $(addprefix +define+,$(DEFINES)) -CFLAGS "-I$(CURDIR)/model/tests -I$(CURDIR)/model/include $(addprefix -D, $(DEFINES))"
.PHONY: all run wave lint clean xval xval_all grid correlate sweep figures reproduce

all:
		@mkdir -p $(OBJ)
		@printf "[$(CFG)] Building RTL..."
		@verilator $(VFLAGS) $(RTL) $(TB) > $(OBJ)/build.log
		@echo "done"

run: all
		./$(SIM) $(W)

correlate:
		@python3 scripts/correlate.py

figures:
		@python3 scripts/plot.py

sweep:
		@$(MAKE) -s --no-print-directory -C model
		@python3 scripts/sweep.py

grid:
		@python3 scripts/grid.py


reproduce:
		@$(MAKE) -s --no-print-directory xval_all
		@$(MAKE) -s --no-print-directory correlate
		@$(MAKE) -s --no-print-directory sweep
		@$(MAKE) -s --no-print-directory figures

xval: all
		@$(MAKE) -s --no-print-directory -C model
		@mkdir -p $(XVAL)/cpp $(XVAL)/rtl
		@printf "[$(CFG)] running: "
		@for w in $(WORKLOADS); do \
			printf " $$w"; \
			./model/model $$w $(MODEL_ARGS) > $(XVAL)/cpp/$$w.json 2> $(XVAL)/cpp/$$w.log || exit 1; \
			./$(SIM) $$w > $(XVAL)/rtl/$$w.json 2>$(XVAL)/rtl/$$w.log || { echo ""; echo "RTL FAILED: $$w"; cat  $(XVAL)/rtl/$$w.log; exit 1; }; \
	done
	@echo ""
	@python3 scripts/compare.py $(XVAL)

xval_all: 
		@$(MAKE) -s --no-print-directory xval WAYS=4 LINE_BYTES=64 CACHE_BYTES=8192
		@$(MAKE) -s --no-print-directory xval WAYS=1 LINE_BYTES=64 CACHE_BYTES=8192
		@$(MAKE) -s --no-print-directory xval WAYS=2 LINE_BYTES=64 CACHE_BYTES=8192
		@$(MAKE) -s --no-print-directory xval WAYS=8 LINE_BYTES=64 CACHE_BYTES=8192
		@$(MAKE) -s --no-print-directory xval WAYS=4 LINE_BYTES=32 CACHE_BYTES=8192
		@$(MAKE) -s --no-print-directory xval WAYS=4 LINE_BYTES=128 CACHE_BYTES=8192
		@$(MAKE) -s --no-print-directory xval WAYS=4 LINE_BYTES=64 CACHE_BYTES=2048
		@$(MAKE) -s --no-print-directory xval WAYS=4 LINE_BYTES=64 CACHE_BYTES=32768
		@echo ""
		@python3 scripts/grid.py
wave: run
	gtkwave wave.vcd

lint:
	verilator --lint-only --assert -Wall -y rtl --top-module $(TOP) $(RTL)

clean:
	rm -rf obj_dir wave.vcd results/xval results/sweeps results/cycles_after.csv