#############################################################################################################
# DESTINATIONS ##############################################################################################
#############################################################################################################

DEST_HEADERS := /usr/include/cassette
DEST_LIBS    := /usr/lib
DEST_BUILD   := build

#############################################################################################################
# INTERNAL VARIABLES ########################################################################################
#############################################################################################################

DIR_DEMOS := examples
DIR_SRC   := src
DIR_INC   := include
DIR_LIB   := $(DEST_BUILD)/lib
DIR_OBJ   := $(DEST_BUILD)/obj
DIR_BIN   := $(DEST_BUILD)/bin

LIST_DEMOS := $(wildcard $(DIR_DEMOS)/*.c)
LIST_SRC   := $(wildcard $(DIR_SRC)/*.c)
LIST_HEAD  := $(wildcard $(DIR_SRC)/*.h) $(wildcard $(DIR_INC)/*.h)
LIST_OBJ   := $(patsubst $(DIR_SRC)/%.c, $(DIR_OBJ)/%.o, $(LIST_SRC))
LIST_BIN   := $(patsubst $(DIR_DEMOS)/%.c, $(DIR_BIN)/%, $(LIST_DEMOS))

OUTPUT := ccfg
FLAGS  := -std=c99 -pedantic -Wall -Wextra -O3 -D_POSIX_C_SOURCE=200809L
LIBS   := -lcobj -lm -lpthread

#############################################################################################################
# PUBLIC TARGETS ############################################################################################
#############################################################################################################

all: lib examples

lib: --prep_lib $(LIST_OBJ)
	cc -shared $(DIR_OBJ)/*.o -o $(DIR_LIB)/lib$(OUTPUT).so $(DIR_LIBS)
	ar rcs $(DIR_LIB)/lib$(OUTPUT).a $(DIR_OBJ)/*.o

examples: --prep_lib --prep_examples lib $(LIST_BIN)

install:
	mkdir -p $(DEST_HEADERS)
	cp $(DIR_INC)/*/* $(DEST_HEADERS)/
	cp $(DIR_LIB)/* $(DEST_LIBS)/

clean:
	rm -rf $(DEST_BUILD)
	rm $(DIR_DEMOS)/config.h

force: clean all

#############################################################################################################
# PRIVATE TARGETS ###########################################################################################
#############################################################################################################

--prep_lib:
	mkdir -p $(DIR_LIB)
	mkdir -p $(DIR_OBJ)

--prep_examples:
	mkdir -p $(DIR_BIN)
	xxd -i $(DIR_DEMOS)/config > $(DIR_DEMOS)/config.h

$(DIR_OBJ)/%.o: $(DIR_SRC)/%.c $(LIST_HEAD)
	$(CC) -c -fPIC $(FLAGS) -c $< -o $@ -I$(DIR_INC) $(LIBS)

$(DIR_BIN)%: $(DIR_DEMOS)/%.c
	$(CC) -static $(FLAGS) $< -o $@ -I$(DIR_INC) -L$(DIR_LIB) -l$(OUTPUT) $(LIBS)
