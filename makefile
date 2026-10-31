# VC6 makefile

BIN_DIR = bin
INCLUDE_DIR = include
OBJ_DIR = obj
RC_DIR = rc
SRC_DIR = src

RC_FILE = $(RC_DIR)\main.rc

RES_FILE = $(OBJ_DIR)\main.RES
OBJ_FILES = $(OBJ_DIR)\main.obj

OUTPUT_FILE = $(BIN_DIR)\LightingBugFix.dll

C_FLAGS = /c /O2 /nologo /W3 /WX /LD /MD
LD_FLAGS = /DLL /FILEALIGN:512 /NOLOGO /RELEASE

$(OUTPUT_FILE): $(OBJ_FILES) $(RES_FILE) $(BIN_DIR)
    link $(OBJ_FILES) $(RES_FILE) $(LD_FLAGS) /OUT:$(OUTPUT_FILE)

{$(SRC_DIR)}.c{$(OBJ_DIR)}.obj::
    $(CC) $(C_FLAGS) $< -I$(INCLUDE_DIR) /Fo./$(OBJ_DIR)/

$(OBJ_FILES): makefile

$(RES_FILE): $(RC_FILE) $(OBJ_DIR) makefile
    rc /fo $(RES_FILE) $(RC_FILE)

$(OBJ_DIR):
    if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)

$(BIN_DIR):
    if not exist $(BIN_DIR) mkdir $(BIN_DIR)

clean:
    del $(BIN_DIR)\*.dll $(OBJ_DIR)\*.obj $(OBJ_DIR)\*.RES
