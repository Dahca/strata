SPDK_ROOT_DIR := $(abspath $(CURDIR)/../../../lib/spdk/)
MLFS_DIR := $(abspath $(CURDIR)/../../)
include $(SPDK_ROOT_DIR)/mk/spdk.common.mk

# (iangneal): override in libfs/Makefile
MLFS_FLAGS +=
ifndef PREFIX
    $(error 'must pass in $$(PREFIX)')
endif
C_SRCS := common.c sync.c async.c
OBJS=$(C_SRCS:.c=.o)
#MLFS_DEP_OBJS = $(MLFS_DIR)/../$(PREFIX)/global/util.o

#OBJS += $(MLFS_DEP_OBJS)

CFLAGS += $(MLFS_FLAGS) -I. -I $(MLFS_DIR) $(ENV_CFLAGS) -I $(SPDK_ROOT_DIR)/include
OPT_ARGS=-g -O3

SPDK_LIBS += $(SPDK_ROOT_DIR)/build/lib/libspdk_nvme.a \
			 $(SPDK_ROOT_DIR)/build/lib/libspdk_util.a \
			 $(SPDK_ROOT_DIR)/build/lib/libspdk_log.a \
			 $(SPDK_ROOT_DIR)/dpdk/build/lib/librte_eal.a

LIBS += $(SPDK_LIBS) $(ENV_LINKER_ARGS)

all: libspdk.so

verify:
	gcc -o verify verify.c $(MLFS_FLAGS) -L. -lspdk

libspdk.so: $(OBJS) $(ENV_LIBS)
	@echo "build shared library"
	$(CC) -shared -fPIC -o libspdk.so -Wl,--whole-archive $(SPDK_LIBS) \
	-Wl,--no-whole-archive $^ -ldl -lrt -lm $(OPT_ARGS)

clean :
	$(CLEAN_C)
	@rm -f libspdk.so

%.o: %.c
	$(CC) -o $@ $(OPT_ARGS) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(SPDK_LIBS) $(SYS_LIBS)  \
	-fPIC -I $(SPDK_ROOT_DIR)/include -I $(API_DIR) -c $< -DMLFS_INFO $(MLFS_FLAGS)

include $(SPDK_ROOT_DIR)/mk/spdk.deps.mk
