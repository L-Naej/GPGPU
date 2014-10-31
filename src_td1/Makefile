################################################################################
#
# Build script for project
#
################################################################################
# $(BIN) est la nom du binaire généré
BIN=aSimpleGLSLProg
# FLAG
FLAGS=-Wall -O3
# INCLUDES
INC=
# SOURCES
SRC=src
# Librairies
LIBDIR=
# Librairies
LIBS=-lrt -lGLEW -lglut -lGLU -lGL -ljpeg
# Librairies
OBJDIR=obj
# OBJET Directory
NVCC=nvcc
GCC=gcc
GPP=g++
# $(OBJECTS) sont les objets des fichiers C / C++ standard qui seront générés après la compilation
OBJECTS=$(OBJDIR)/main.o $(OBJDIR)/interface.o $(OBJDIR)/display.o $(OBJDIR)/rtfbo.o $(OBJDIR)/imageComputation.o

# all est la première règle à être exécutée car elle est la première
# dans le fichier Makefile. Notons que les dépendances peuvent être
# remplacées par une variable, ainsi que n'importe quel chaine de
# caractères des commandes
all: $(OBJECTS)
	@echo "**** PHASE DE LIEN ****"
	$(GPP) $(OBJECTS) -o $(BIN) $(LIBDIR) $(LIBS) 

obj/main.o: $(SRC)/main.cpp
	@echo "**** $@ ****"
	$(GPP) $(INC) $(FLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRC)/%.cpp $(SRC)/%.hpp
	@echo "**** $@ ****"
	$(GPP) $(INC) $(FLAGS) -c $< -o $@

clean:
	- rm $(OBJDIR)/*.o
	- rm $(BIN)


