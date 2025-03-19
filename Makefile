SRC_DIR   := ./src
INC_DIR   := ./inc
BLD_DIR   := ./bin
VAL_DIR   := ./validations
TARGET    := $(BLD_DIR)/mandelbrot
SRCS      := $(sort $(shell find $(SRC_DIR) -name '*.cpp' -printf '%p '))
OBJS      := $(subst $(SRC_DIR),$(BLD_DIR),$(SRCS:%.cpp=%.o))
DEPS      := $(OBJS:.o=.d)
INC_FLAGS := $(addprefix -I,$(INC_DIR))
CPPFLAGS  := -MMD -MP
LDFLAGS   += -lX11
CXXFLAGS  += -Wall -Werror $(INC_FLAGS) -g -fdiagnostics-color=always

ifeq ($(DEBUG), 1)
CXXFLAGS += -DDEBUG_GRFX
endif

.PHONY: all

all: build

$(BLD_DIR):
	mkdir -p $(BLD_DIR)

$(BLD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BLD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS)

build: $(TARGET)

rebuild: clean build

clean:
	rm -rf $(BLD_DIR) $(VAL_DIR)

print:
	@echo "ARCH='$(ARCH)'"
	@echo "CROSS_COMPILE='$(CROSS_COMPILE)'"
	@echo "TARGET='$(TARGET)'"
	@echo "SRCS='$(SRCS)'"
	@echo "OBJS='$(OBJS)'"
	@echo "DEPS='$(DEPS)'"
	@echo "CXX='$(CXX)'"
	@echo "FLAGS='$(CPPFLAGS)' '$(CXXFLAGS)' '$(LDFLAGS)'"

-include $(DEPS)
