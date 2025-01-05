CUBE_HANDLE := ./scripts/get_cube_api.py
OS_CUBE_JSON ?= ./build/os_cube_api.json
APP_CUBE_JSON := ./build/app_cube_api.json

os_cube:
	@echo "====== build OS cube api ======="
	@python3 $(CUBE_HANDLE) $(STATIC_OBJS_DIR) $(OS_CUBE_JSON)
	@echo "------ build OS cube api end ---------"

app_cube:
	@echo "====== build APP cube api ======="
	@python3 $(CUBE_HANDLE) $(STATIC_OBJS_DIR) $(APP_CUBE_JSON) $(OS_CUBE_JSON)
	@echo "------ build APP cube api end ---------"


os_cube: xmake_inm
app_cube: xmake_inm
os_files: os_cube
