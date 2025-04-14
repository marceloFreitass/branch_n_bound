#detecta se o sistema é de 32 ou 64 bits
BITS_OPTION = -m64

####diretorios com as libs do cplex

GUROBI_DIR =/opt/gurobi1201/linux64

#### define o compilador
CPPC = g++
#############################

#### opcoes de compilacao e includes
CCOPT = $(BITS_OPTION) -fPIC -fexceptions -DIL_STD $(CFLAGS_MODE) 
GUROBIINCDIR = $(GUROBI_DIR)/include/
GUROBICPPLIB = -L$(GUROBI_DIR)/lib -lgurobi_c++ -lgurobi120
CCFLAGS = $(CCOPT) -I$(GUROBIINCDIR) -MMD -Wall
#############################

SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))

#### flags do linker
CCLNFLAGS = -L$(GUROBI_DIR)/lib -lgurobi_c++ -lgurobi120 -lm -lpthread -ldl 
#############################

#### diretorios com os source files e com os objs files
SRCDIR = src
OBJDIR = obj
#############################
BUILD ?= release

ifeq ($(BUILD),release)
    CFLAGS_MODE = -O3 -DNDEBUG
    OBJDIR = obj/release
else ifeq ($(BUILD),debug)
    CFLAGS_MODE = -g -D _GLIBCXX_DEBUG -D _GLIBCXX_DEBUG_PEDANTIC
    OBJDIR = obj/debug
else
    $(error Invalid BUILD mode '$(BUILD)'. Use 'debug' or 'release')
endif

#### lista de todos os srcs e todos os objs
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))
#############################

#### regra principal, gera o executavel
bb: $(OBJS) 
	@echo  "\033[31m \nLinking all objects files: \033[0m"
	$(CPPC) $(BITS_OPTION) $(OBJS) -o $@ $(CCLNFLAGS)
############################

#inclui os arquivos de dependencias
-include $(OBJS:.o=.d)

#regra para cada arquivo objeto: compila e gera o arquivo de dependencias do arquivo objeto
#cada arquivo objeto depende do .c e dos headers (informacao dos header esta no arquivo de dependencias gerado pelo compiler)
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | mkdirs
	@echo  "\033[31m \nCompiling $< [$(BUILD) mode]: \033[0m"
	$(CPPC) $(CCFLAGS) -c $< -o $@
#delete objetos e arquivos de dependencia
clean:
	@echo "\033[31mCleaning all obj directories\033[0m"
	@rm -f bb
	@rm -rf obj/debug obj/release

mkdirs:
	mkdir -p $(OBJDIR)
	
rebuild: 
	$(MAKE) clean
	$(MAKE) bb
debug:
	$(MAKE) BUILD=debug bb

release:
	$(MAKE) BUILD=release bb
