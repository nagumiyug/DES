BUILD_DIR = build
INC_DIR = inc
SRC_DIR = src

all:
	gcc \
		-Wall -Wextra           \
		-O3 -funroll-loops      \
		$(SRC_DIR)/*		    \
		-o $(BUILD_DIR)/main 
clean:
	rm -rf $(BUILD_DIR)/*