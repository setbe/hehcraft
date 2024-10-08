PROJECT_NAME := hehcraft

BUILD_DIR := build
BINARY := $(BUILD_DIR)/$(PROJECT_NAME)

.PHONY: default clean rebuild

default:
	@if [ ! -d $(BUILD_DIR) ]; then mkdir $(BUILD_DIR); fi
	@cd $(BUILD_DIR) && cmake ..
	@cd $(BUILD_DIR) && make

clean:
	@rm -rf $(BUILD_DIR)

rebuild: clean
	@mkdir $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake ..
	@cd $(BUILD_DIR) && make

minsize: clean
	@mkdir $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. -DCMAKE_BUILD_TYPE=MinSizeRel
	@cd $(BUILD_DIR) && cmake --build .
	@strip $(BINARY)

release: clean
	@mkdir $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. -DCMAKE_BUILD_TYPE=Release
	@cd $(BUILD_DIR) && make

debug: clean
	@mkdir $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. -DCMAKE_BUILD_TYPE=Debug
	@cd $(BUILD_DIR) && make

run: default
	@cd $(BUILD_DIR) && ./$(PROJECT_NAME)