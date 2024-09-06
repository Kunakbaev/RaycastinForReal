LIBS := -lsfml-graphics -lsfml-window -lsfml-system

CC 	   := g++
CFLAGS := -D _DEBUG -lm -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -pie -fPIE -Werror=vla
# CFLAGS = -D _DEBUG
CFLAGS += -fsanitize=address

SOURCE_DIR	 		:= source
BUILD_DIR    		:= building
LIB_RUN_NAME 		:= raycastingRun
ASSERT_DEFINE		:=
SFML_PATH			:= -I/libs/sfml/include

.PHONY: $(LIB_RUN_NAME) run $(BUILD_DIR) clean


SRC := $(wildcard ./$(SOURCE_DIR)/*.cpp)
OBJ := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(notdir ${SRC}))

$(LIB_RUN_NAME): $(OBJ)
	#  Prerequisites: sudo apt-get install libsfml-dev
	@$(CC) $^ -o $(BUILD_DIR)/$(LIB_RUN_NAME) $(LIBS) $(CFLAGS)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp $(BUILD_DIR)
	@$(CC) -c $< $(CFLAGS) -o $@ $(ASSERT_DEFINE)

run: $(LIB_RUN_NAME)
	$(BUILD_DIR)/$(LIB_RUN_NAME)

# -------------------------   HELPER TARGETS   ---------------------------

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
clean:
	rm -f $(BUILD_DIR)/*.o libRun
