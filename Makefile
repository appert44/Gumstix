##############################################################################
# Makefile pour generer : nom de l'application
# Projet : nom du projet
# Date : 
# Commande : make
#############################################################################

# Dossier pour les fichiers sources
SRCDIR = Classes

# Dossier pour les compilations
BUILDDIR = build

# Dossier pour les fichiers objets (.o)
OBJDIR = $(BUILDDIR)/.obj

# Dossier pour les exécutables de test unitaire
UNITTESTDIR = $(BUILDDIR)/UnitTest
UNITTESTBIN = $(UNITTESTDIR)/bin

# Dossier pour les exécutable des releases
RELEASEDIR = $(BUILDDIR)/Release
RELEASEBIN = $(RELEASEDIR)/bin

# Dossier de la documentation
DOCDIR = $(BUILDDIR)/html

# Dossier d'installation
INSTALLDIR = ~/bin

# Options de compilation à ajouter à CFLAGS si nécessaire
SVNDEF = -D_SVN_=\"$(shell svnversion -n .)\"

DAYOFWEEKDEF = -D_DAYOFWEEK_=\"$(shell date +%a)\"
DAYDEF = -D_DAY_=\"$(shell date +%d)\"
MONTHDEF = -D_MONTH_=\"$(shell date +%m)\"
YEARDEF = -D_YEAR_=\"$(shell date +%y)\"
HOURDEF = -D_HOUR_=\"$(shell date +%H)\"
MINDEF = -D_MIN_=\"$(shell date +%M)\"

DEBUG_ON = -g
WARNING_ON = -Wall

CFLAG=${CFLAGS} $(DEBUG_ON) $(WARNING_ON)
CXXFLAG=${CXXFLAGS} $(DEBUG_ON) $(WARNING_ON)

# Options de recher(OBJDIR)ches des includes
INCLUDES = -I overo/usr/include 

# Définitions à la compilation (ex -DDEBUG = #define DEBUG)
DEFINES = 

# Options de linkage si necessaire (ex -lm)
LIBSTEST = -lboost_thread -lboost_system -lpthread $(HOME)/overo/usr/lib/libcppunit.so $(HOME)/overo/usr/lib/libdl.so
LIBS = -lboost_thread -lboost_system -lpthread

# Commande de compilation C++
#CC = arm-poky-linux-gnueabi-g++ $(CFLAGS) $(INCLUDES) $(DEFINES)
CC=${CXX} $(CXXFLAG) $(INCLUDES) $(DEFINES)
# Commande de compilation C
C = gcc $(CFLAG) $(INCLUDES) $(DEFINES)

# Liste fichiers source
SRC = $(wildcard $(SRCDIR)/*.m)
HDR = $(wildcard $(SRCDIR)/*.h)

# On place ici les objets (.o) a linker dans les applications finales

# Classes auxiliaires
# ex                    = $(OBJDIR)/MyAuxClass.o
AUX_OBJECTS		=		$(OBJDIR)/HTTPClient.o \
						$(OBJDIR)/LM74.o \
					$(OBJDIR)/CIODevice.o \
				        $(OBJDIR)/CEvent.o \
				        $(OBJDIR)/CGPIO.o \
					$(OBJDIR)/PIRSTDLPSensor.o \


# Classes de tests unitaires
# ex                    = $(OBJDIR)/MyAuxClassTest.o 
UNITTEST_OBJECTS	=	$(OBJDIR)/HTTPClientTest.o \
						$(OBJDIR)/LM74Test.o \
				$(OBJDIR)/CGPIOTest.o \
				$(OBJDIR)/PIRSTDLPSensorTest.o \

# Classe principale de release
# ex                    = $(OBJDIR)/MyMainClass.o
RELEASE_OBJECTS		= 	$(OBJDIR)/Gestionnaire.o \

# Objet à générer
OBJECTS		= 	$(AUX_OBJECTS) $(UNITTEST_OBJECTS) $(RELEASE_OBJECTS)

# On place ici les exécutables à générer (testsunitaires et release)
# ex            = 	$(UNITTESTBIN)/MyAuxClassTest
UNITTEST	=	$(UNITTESTBIN)/HTTPClientTest \
			$(UNITTESTBIN)/LM74Test \
			$(UNITTESTBIN)/CGPIOTest \
			$(UNITTESTBIN)/PIRSTDLPSensorTest \

# ex            = 	$(RELEASEBIN)/MyMainClass
RELEASE		=	$(RELEASEBIN)/Gestionnaire \

# On rajoute ici ce qui doit être généré
all : $(BUILDDIR) $(OBJECTS) $(RELEASE) $(UNITTEST)

# Cibles pour le nettoyage de la compilation
cleanall : clean

clean :
	@rm -f $(OBJDIR)/* $(DOCDIR)/* 

# Cible pour l'enchaînement des tests unitaires (à venir)
tests: test-HTTPClientTest

# Cible pour l'analyse du code source (à venir)
analyze :

# Cible pour la génération de la documentation du code
doc:
	doxygen ./Doxyfile

# Install
install : $(INSTALLDIR)
	cp $(RELEASEBIN)/* $(INSTALLDIR)/.

# Install test
installtest : $(INSTALLDIR)
	cp $(UNITTESTBIN)/* $(INSTALLDIR)/.

# Production des .o (c++)
$(OBJDIR)/%.o :  $(SRCDIR)/%.cpp $(SRCDIR)/%.h
	$(CC) -c $< -o $@

# Production des .o si pas de .h (c++)
$(OBJDIR)/%.o :  $(SRCDIR)/%.cpp
	$(CC) -c $< -o $@

# Production des tests unitaires
$(UNITTESTBIN)/HTTPClientTest :	$(AUX_OBJECTS) $(OBJDIR)/HTTPClientTest.o
	$(CC) $^  $(LIBSTEST) -o $@
$(UNITTESTBIN)/LM74Test :	$(AUX_OBJECTS) $(OBJDIR)/LM74Test.o
	$(CC) $^  $(LIBSTEST) -o $@
$(UNITTESTBIN)/CGPIOTest :	$(AUX_OBJECTS) $(OBJDIR)/CGPIOTest.o
	$(CC) $^  $(LIBSTEST) -o $@
$(UNITTESTBIN)/PIRSTDLPSensorTest :	$(AUX_OBJECTS) $(OBJDIR)/PIRSTDLPSensorTest.o
	$(CC) $^  $(LIBSTEST) -o $@



# Production de la release
$(RELEASEBIN)/Gestionnaire : $(AUX_OBJECTS) $(OBJDIR)/Gestionnaire.o
	$(CC) $^ $(LIBS) -o $@


$(INSTALLDIR) :
	mkdir -p $@

# Création de l'arborescence pour le build
$(BUILDDIR):
	mkdir $@ && \
	mkdir $@/.obj && \
	mkdir $@/html && \
	mkdir $@/UnitTest && \
	mkdir $@/UnitTest/bin && \
	mkdir $@/UnitTest/configuration && \
	mkdir $@/UnitTest/logs && \
	mkdir $@/Release && \
	mkdir $@/Release/bin && \
	mkdir $@/Release/configuration && \
	mkdir $@/Release/logs

test-HTTPClientTest :
	[ -e Makefile ] && (cd $(UNITTESTBIN) && ./HTTPClientTest) || true

	
