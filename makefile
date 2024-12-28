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

CPP_FLAGS = /c /O2 /nologo /W3 /WX /LD /MD /EHsc
LD_FLAGS = /DLL /NOLOGO /RELEASE
LIB_FLAGS = /NOLOGO /MACHINE:IX86

$(OUTPUT_FILE): $(OBJ_FILES) $(RES_FILE) $(BIN_DIR)
    link $(OBJ_FILES) $(RES_FILE) $(LD_FLAGS) /OUT:$(OUTPUT_FILE)

{$(SRC_DIR)}.cpp{$(OBJ_DIR)}.obj::
    $(CPP) $(CPP_FLAGS) $< -I$(INCLUDE_DIR) /Fo./$(OBJ_DIR)/

$(OBJ_FILES): makefile

$(RES_FILE): $(RC_FILE) $(OBJ_DIR) makefile
    rc /fo $(RES_FILE) $(RC_FILE)

$(OBJ_DIR):
    if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)

$(BIN_DIR):
    if not exist $(BIN_DIR) mkdir $(BIN_DIR)

clean:
    del $(BIN_DIR)\*.dll $(OBJ_DIR)\*.obj $(OBJ_DIR)\*.RES
