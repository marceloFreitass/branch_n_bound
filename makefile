#detecta se o sistema é de 32 ou 64 bits
BITS_OPTION = -m64

####diretorios com as libs do cplex

GUROBI_DIR =/opt/gurobi1201/linux64

#### define o compilador
CPPC = g++
#############################

#### opcoes de compilacao e includes
CCOPT = $(BITS_OPTION) -O3 -fPIC -fexceptions -DNDEBUG -DIL_STD 
GUROBIINCDIR = $(GUROBI_DIR)/include/
GUROBICPPLIB = -L$(GUROBI_DIR)/lib -lgurobi_c++ -lgurobi120
CCFLAGS = -I$(GUROBIINCDIR) -MMD -Wall
#############################

#### flags do linker
CCLNFLAGS = -L$(GUROBI_DIR)/lib -lgurobi_c++ -lgurobi120 -lm -lpthread -ldl 
#############################

#### diretorios com os source files e com os objs files
SRCDIR = src
OBJDIR = obj
#############################

#### lista de todos os srcs e todos os objs
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))
#############################

#### regra principal, gera o executavel
bc: 
	$(MAKE) mkdirs
	$(MAKE) $(OBJS) 
	@echo  "\033[31m \nLinking all objects files: \033[0m"
	$(CPPC) $(BITS_OPTION) $(OBJS) -o $@ $(CCLNFLAGS)
############################

#inclui os arquivos de dependencias
-include $(OBJS:.o=.d)

#regra para cada arquivo objeto: compila e gera o arquivo de dependencias do arquivo objeto
#cada arquivo objeto depende do .c e dos headers (informacao dos header esta no arquivo de dependencias gerado pelo compiler)
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo  "\033[31m \nCompiling $<: \033[0m"
	$(CPPC) $(CCFLAGS) -c $< -o $@
#delete objetos e arquivos de dependencia
clean:
	@echo "\033[31mcleaning obj directory \033[0m"
	@rm bc -f $(OBJDIR)/*.o $(OBJDIR)/*.d

mkdirs:
	mkdir -p obj
	
rebuild: 
	$(MAKE) clean
	$(MAKE) bc
