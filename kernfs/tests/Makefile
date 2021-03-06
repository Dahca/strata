CC=gcc9.2
CXX=g++ -std=c++11

#CC = clang-3.8
#CXX = clang++-3.8

########
#  Lib directories
########
LIBFS_DIR := $(CURDIR)/../../libfs
#NVML_DIR := $(abspath $(LIBFS_DIR)/lib/nvml/src)
NVML_DIR :=
DPDK_DIR := $(abspath $(LIBFS_DIR)/lib/dpdk-16.07/x86_64-native-linuxapp-gcc)
SPDK_DIR := $(abspath $(LIBFS_DIR)/lib/spdk)
CUCKOO_DIR := $(abspath $(LIBFS_DIR)/lib/cuckoofilter)
#GLIB_DIR := $(abspath $(LIBFS_DIR)/lib/glib)
GLIB_DIR :=
LIBSPDK_DIR := $(abspath $(LIBFS_DIR)/src/storage/spdk/)
C_LIBCUCKOO_DIR := $(abspath $(LIBFS_DIR)/lib/libcuckoo-c/src)
API_DIR := $(abspath ../../../nvm-file-indexing/api)

INCLUDES := $(addprefix -I, .. . $(LIBSPDK_DIR) $(NVML_DIR)/include $(CUCKOO_DIR)/src $(C_LIBCUCKOO_DIR) $(GLIB_DIR) $(GLIB_DIR)/glib $(API_DIR))

LD_FLAGS = -lpthread -L$(NVML_DIR)/nondebug/ -lpmem -lrt \
	-Wl,-rpath=$(abspath $(GLIB_DIR)/glib/.libs) -L$(abspath $(GLIB_DIR)/glib/.libs) \
	-Wl,-rpath=$(abspath $(API_DIR)/build) -L$(abspath $(API_DIR)/build) -lNVMIndexingAPI -lm
LD_FLAGS_CXX = -lpthread -lm -L$(NVML_DIR)/nondebug/ -lpmem -lrt \
	-Wl,-rpath=$(abspath $(GLIB_DIR)/glib/.libs) -L$(abspath $(GLIB_DIR)/glib/.libs) \
	-Wl,-rpath=$(abspath $(API_DIR)/build) -L$(abspath $(API_DIR)/build) -lNVMIndexingAPI

#DEBUG = -g -O0
#DEBUG = -g -Ofast
AVX_ARGS = -march=skylake-avx512 -mavx -mavx512f -mavx512vl -mavx512cd -mavx512bw -mavx512dq
DEBUG = -g -O0 $(AVX_ARGS)
OPT_ARGS = -g 03 $(AVX_ARGS)


#MLFS_FLAGS = -DUSE_SLAB -DMLFS_INFO -DKERNFS
MLFS_FLAGS = -DMLFS_INFO -DKERNFS
#MLFS_FLAGS = -DKERNFS
#MLFS_FLAGS += -DBALLOC
#MLFS_FLAGS += -DDIGEST_OPT
#MLFS_FLAGS += -DIOMERGE
MLFS_FLAGS += -DCONCURRENT
MLFS_FLAGS += -DFCONCURRENT
#MLFS_FLAGS += -DUSE_SSD
#MLFS_FLAGS += -DUSE_HDD
#MLFS_FLAGS += -DMIGRATION
#MLFS_FLAGS += -DEXPERIMENTAL
MLFS_FLAGS += -DHASHTABLE

#######
# Optimization definitions
#######
MLFS_FLAGS += -DREUSE_PREVIOUS_PATH  # optimization for extent trees

########
# Testing definitions
########
MLFS_FLAGS += -DSIMULATE_FRAGMENTATION

########
#  Phony targets
########
.PHONY: kernfs all clean

BIN := kernfs fifo_cli extent_test concurrency_test nvram_versus_dram

all: $(BIN)

clean:
	@rm -rfv *.o $(BIN)

scrub: clean
	@rm -rfv *.txt *.out *.log

time_stat.o: time_stat.c
	$(CC) time_stat.c -c -o time_stat.o

nvram_versus_dram: nvram_versus_dram.c time_stat.o
	$(CC) $^ -o $@ -lm

test_gen.o: test_gen.cc
	$(CXX) $^ $(DEBUG) -c -o $@ $(INCLUDES) -L../build -lkernfs -L$(LIBSPDK_DIR) -lspdk $(LD_FLAGS_CXX) -Wl,-rpath=$(abspath ../build) -Wl,-rpath=$(abspath $(LIBSPDK_DIR)) -Wl,-rpath=$(abspath $(NVML_DIR)/nondebug) $(MLFS_FLAGS)

kernfs: kernfs.c
	$(CC) $^ $(DEBUG) -o $@ $(INCLUDES) -L../build -lkernfs -L$(LIBSPDK_DIR) -lspdk $(LD_FLAGS)  -Wl,-rpath=$(abspath $(LIBSPDK_DIR)) -Wl,-rpath=$(abspath $(NVML_DIR)/nondebug) -Wl,-rpath=$(abspath ../build)

extent_test: extent_test.cc time_stat.o test_gen.o
	$(CXX) $^ $(DEBUG) -o $@ $(INCLUDES) -L../hbuild -lkernfs -L$(LIBSPDK_DIR) -lspdk $(LD_FLAGS_CXX) -Wl,-rpath=$(abspath ../build) -Wl,-rpath=$(abspath $(LIBSPDK_DIR)) -Wl,-rpath=$(abspath $(NVML_DIR)/nondebug) $(MLFS_FLAGS)

concurrency_test: concurrency_test.cc time_stat.o test_gen.o
	$(CXX) $^ $(DEBUG) -o $@ $(INCLUDES) -L../build -lkernfs -L$(LIBSPDK_DIR) -lspdk $(LD_FLAGS_CXX) -Wl,-rpath=$(abspath ../build) -Wl,-rpath=$(abspath $(LIBSPDK_DIR)) -Wl,-rpath=$(abspath $(NVML_DIR)/nondebug) $(MLFS_FLAGS)

fifo_cli: fifo_cli.c
	$(CC) -o $@ $^
