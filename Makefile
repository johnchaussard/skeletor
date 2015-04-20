LIB_PINK=$(PINK)/lib

#Quelques librairies externes
INCLUDE_PINK=$(PINK)/include/

#Définitions de dossiers
INC_PERSO_DIR=./include/
OBJ_PERSO_DIR=./obj/
SRC_PERSO_DIR=./src/
BIN_PERSO_DIR=./bin/
LIB_PERSO_DIR=./lib/

#Les fichiers objets
OBJ_FILES=\
$(OBJ_PERSO_DIR)libinventor.o\
$(OBJ_PERSO_DIR)libvrml.o\
$(OBJ_PERSO_DIR)libvskel.o\
$(OBJ_PERSO_DIR)liblist.o\
$(OBJ_PERSO_DIR)libgeneral.o\
$(OBJ_PERSO_DIR)libmin_sphere.o\
$(OBJ_PERSO_DIR)libcrop.o\
$(OBJ_PERSO_DIR)libeps.o

#Les exe
BIN_FILES=\
$(BIN_PERSO_DIR)cca2iv\
$(BIN_PERSO_DIR)cca2eps\
$(BIN_PERSO_DIR)cca_collapse\
$(BIN_PERSO_DIR)cca_filter_surface\
$(BIN_PERSO_DIR)cca_filter_elements\
$(BIN_PERSO_DIR)cca_keepcells\
$(BIN_PERSO_DIR)cca_makecomplex\
$(BIN_PERSO_DIR)cca_printstats\
$(BIN_PERSO_DIR)cca_surfcollapse\
$(BIN_PERSO_DIR)cca_curvcollapse\
$(BIN_PERSO_DIR)khalimsky2cca\
$(BIN_PERSO_DIR)pgm2cca\
$(BIN_PERSO_DIR)pgm2iv\
$(BIN_PERSO_DIR)raw2pgmP5\
$(BIN_PERSO_DIR)cca_compute_rvsa\
$(BIN_PERSO_DIR)cca_vsacollapse_kf\
$(BIN_PERSO_DIR)cca_computesurfaxis\
$(BIN_PERSO_DIR)cca_compute_unidist\
$(BIN_PERSO_DIR)cca_collapse_on_surfintersect\
$(BIN_PERSO_DIR)cca_skel_to_labelled_pgm\
$(BIN_PERSO_DIR)distance_dilat\
$(BIN_PERSO_DIR)distance_dilat_float\
$(BIN_PERSO_DIR)distance_dilatfactor\
$(BIN_PERSO_DIR)distance_dilat_euc\
$(BIN_PERSO_DIR)cca_decompose_surfaces\
$(BIN_PERSO_DIR)cca_decompose_surfaces_advanced\
$(BIN_PERSO_DIR)cca_decompose_surface_overcollapse\
$(BIN_PERSO_DIR)cca_decompose_curve\
$(BIN_PERSO_DIR)cca_decompose_skeleton_into_elements\
$(BIN_PERSO_DIR)cca_surfaxis_keepsingledges\
$(BIN_PERSO_DIR)shrink_on_disk\
$(BIN_PERSO_DIR)cca_collapse_minimizeintersect\
$(BIN_PERSO_DIR)compute_cube_lookup\
$(BIN_PERSO_DIR)prepare_mask_foam10\
$(BIN_PERSO_DIR)cca_and\
$(BIN_PERSO_DIR)cca_or\
$(BIN_PERSO_DIR)cca_sub\
$(BIN_PERSO_DIR)cca_keepfaces\
$(BIN_PERSO_DIR)cca_keepnonregularedges\
$(BIN_PERSO_DIR)cca2khalimsky\
$(BIN_PERSO_DIR)cca_generate_containing_cube\
$(BIN_PERSO_DIR)cca_keepedges\
$(BIN_PERSO_DIR)cca_filter_surfaces_collapse\
$(BIN_PERSO_DIR)cca_filter_surfaces_irregular_border\
$(BIN_PERSO_DIR)cca_filter_intersect_edges\
$(BIN_PERSO_DIR)ddil_square\
$(BIN_PERSO_DIR)expand_regions_count_contacts\
$(BIN_PERSO_DIR)cca_pardircollapse\
$(BIN_PERSO_DIR)cca_pgm_keep_curve_extremities\
$(BIN_PERSO_DIR)cca_crop\
$(BIN_PERSO_DIR)pgmdiv\
$(BIN_PERSO_DIR)cca_geodilat\
$(BIN_PERSO_DIR)overframe\
$(BIN_PERSO_DIR)cca_keepnonregularvertex\
$(BIN_PERSO_DIR)generate_pgm\
$(BIN_PERSO_DIR)copy_into\
$(BIN_PERSO_DIR)cca_compute_salience_map\
$(BIN_PERSO_DIR)crop_on_disk\
$(BIN_PERSO_DIR)pgm_enum\
$(BIN_PERSO_DIR)pgm2double\
$(BIN_PERSO_DIR)distance_between_maps\
$(BIN_PERSO_DIR)long2float\
$(BIN_PERSO_DIR)pgm_draw_containing_cube\
$(BIN_PERSO_DIR)cca_curvskel\
$(BIN_PERSO_DIR)cca_surfskel\
$(BIN_PERSO_DIR)cca_curvskel_deep\
$(BIN_PERSO_DIR)cca_surfskel_deep\
$(BIN_PERSO_DIR)pgm_generate_lambda_skelprio\
$(BIN_PERSO_DIR)pgm_make_coarser_decomposition\
$(BIN_PERSO_DIR)div2\
$(BIN_PERSO_DIR)pgm_curvskel\
$(BIN_PERSO_DIR)pgm_curvskelp\
$(BIN_PERSO_DIR)pgm_surfskel\
$(BIN_PERSO_DIR)cca_decompose_volume\
$(BIN_PERSO_DIR)pgmlabel2iv\
$(BIN_PERSO_DIR)cca_curvaxis\
$(BIN_PERSO_DIR)cca_surfaxis\
$(BIN_PERSO_DIR)pgmlabel_intersect\
$(BIN_PERSO_DIR)cca2vskel\
$(BIN_PERSO_DIR)cca_dist_to_vertex\




#...et sa dépendance...
LIB_DPD_LIBCOMPLEX=\
$(SRC_PERSO_DIR)lib/libcomplex_collapse.cxx\
$(SRC_PERSO_DIR)lib/libcomplex_convert.cxx\
$(SRC_PERSO_DIR)lib/libcomplex_data.cxx\
$(SRC_PERSO_DIR)lib/libcomplex_general.cxx\
$(SRC_PERSO_DIR)lib/libcomplex_surfaces.cxx\
$(SRC_PERSO_DIR)lib/libcomplex_dist.cxx\
$(INC_PERSO_DIR)libcomplex.h\
$(INC_PERSO_DIR)libcomplex_d.h\
$(OBJ_PERSO_DIR)libinventor.o\
$(OBJ_PERSO_DIR)libvrml.o\
$(OBJ_PERSO_DIR)libvskel.o\
$(OBJ_PERSO_DIR)liblist.o\
$(OBJ_PERSO_DIR)libeps.o\

#...et ses constituants
LIB_OBJ_LIBCOMPLEX=\
$(OBJ_PERSO_DIR)libcomplex_collapse.o\
$(OBJ_PERSO_DIR)libcomplex_convert.o\
$(OBJ_PERSO_DIR)libcomplex_data.o\
$(OBJ_PERSO_DIR)libcomplex_general.o\
$(OBJ_PERSO_DIR)libcomplex_dist.o\
$(OBJ_PERSO_DIR)libcomplex_surfaces.o\




#Le compilateur et ses options
CC=g++
LC1=ar
LC2=ranlib
PINKFLAGS=-DMC_64_BITS -DUNIXIO -DPC -DHAVE_POSIX_IPC -DHAVE_SYSV_IPC -DHAVE_TIFF_LIB -DPINK_HAVE_SIMD -DUNIXIO -DPC
CCFLAGS=$(PINKFLAGS) -g -c -I $(INC_PERSO_DIR) -I $(INCLUDE_PINK)
LDFLAGS=$(PINKFLAGS) -g -I $(INC_PERSO_DIR) -I $(INCLUDE_PINK)
ADDLIBS=$(OBJ_FILES) -L$(LIB_PERSO_DIR) -L$(LIB_PINK) -lcomplex -lpink -lm



#Les différentes options de makefile

all: $(OBJ_FILES) $(BIN_FILES) $(LIB_PERSO_DIR)libcomplextotal.a

uninstall:
	@rm -rf $(BIN_PERSO_DIR)*
	@rm -rf $(LIB_PERSO_DIR)libcomplextotal.a
	@make clean

clean:
	@rm -rf $(OBJ_PERSO_DIR)*.o $(LIB_PERSO_DIR)libcomplex.a


#Dépendances des fichiers

#Les bins
$(BIN_PERSO_DIR)khalimsky2cca: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)cca2khalimsky: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)cca_keepedges: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)pgm2cca: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)cca2iv: $(LIB_PERSO_DIR)libcomplex.a $(OBJ_PERSO_DIR)libinventor.o $(OBJ_PERSO_DIR)libvskel.o

$(BIN_PERSO_DIR)cca2vskel: $(LIB_PERSO_DIR)libcomplex.a $(OBJ_PERSO_DIR)libinventor.o $(OBJ_PERSO_DIR)libvskel.o

$(BIN_PERSO_DIR)cca2eps: $(OBJ_PERSO_DIR)libeps.o $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)cca_decompose_surfaces: $(LIB_PERSO_DIR)libcomplex.a $(OBJ_PERSO_DIR)libinventor.o $(OBJ_PERSO_DIR)liblist.o

$(BIN_PERSO_DIR)cca_decompose_surfaces_advanced: $(LIB_PERSO_DIR)libcomplex.a $(OBJ_PERSO_DIR)libinventor.o $(OBJ_PERSO_DIR)liblist.o

$(BIN_PERSO_DIR)cca_decompose_curve: $(LIB_PERSO_DIR)libcomplex.a $(OBJ_PERSO_DIR)libinventor.o $(OBJ_PERSO_DIR)liblist.o

$(BIN_PERSO_DIR)cca_decompose_skeleton_into_elements: $(LIB_PERSO_DIR)libcomplex.a $(OBJ_PERSO_DIR)libinventor.o $(OBJ_PERSO_DIR)liblist.o

$(BIN_PERSO_DIR)cca_filter_surfaces_collapse: $(LIB_PERSO_DIR)libcomplex.a $(OBJ_PERSO_DIR)liblist.o

$(BIN_PERSO_DIR)cca_filter_elements: $(LIB_PERSO_DIR)libcomplex.a $(OBJ_PERSO_DIR)liblist.o

$(BIN_PERSO_DIR)cca_filter_surfaces_irregular_border: $(LIB_PERSO_DIR)libcomplex.a $(OBJ_PERSO_DIR)liblist.o $(INC_PERSO_DIR)libgeneral.h

$(BIN_PERSO_DIR)cca_filter_intersect_edges: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)pgm2iv: $(LIB_PERSO_DIR)libcomplex.a $(OBJ_PERSO_DIR)libinventor.o

$(BIN_PERSO_DIR)cca_surfcollapse: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)cca_compute_salience_map: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)cca_vsacollapse_kf: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)cca_collapse: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)cca_printstats: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)cca_makecomplex: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)cca_keepcells: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)cca_keepnonregularedges: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)cca_keepnonregularvertex: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)cca_filter_surface: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)cca_compute_rvsa: $(LIB_PERSO_DIR)libcomplex.a $(OBJ_PERSO_DIR)libinventor.o

$(BIN_PERSO_DIR)cca_computesurfaxis: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)cca_pardircollapse: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)cca_curvcollapse: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)cca_curvskel: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)pgm_curvskel: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)pgm_curvskelp: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)cca_curvskel_deep: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)cca_surfskel: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)pgm_surfskel: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)cca_surfskel_deep: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)cca_collapse_on_surfintersect: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)cca_surfaxis_keepsingledges: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)cca_collapse_minimizeintersect: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)cca_decompose_surfaces: $(LIB_PERSO_DIR)libcomplex.a $(OBJ_PERSO_DIR)libinventor.o

$(BIN_PERSO_DIR)prepare_mask_foam10: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)cca_generate_containing_cube: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)expand_regions_count_contacts: $(OBJ_PERSO_DIR)liblist.o

$(BIN_PERSO_DIR)cca_pgm_keep_curve_extremities: $(LIB_PERSO_DIR)libcomplex.a $(OBJ_PERSO_DIR)liblist.o

$(BIN_PERSO_DIR)cca_crop: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)cca_geodilat: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)cca_decompose_volume: $(LIB_PERSO_DIR)libcomplex.a $(OBJ_PERSO_DIR)liblist.o

$(BIN_PERSO_DIR)cca_skel_to_labelled_pgm: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)pgmlabel2iv: $(LIB_PERSO_DIR)libcomplex.a $(OBJ_PERSO_DIR)libinventor.o

$(BIN_PERSO_DIR)cca_curvaxis: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)cca_surfaxis: $(LIB_PERSO_DIR)libcomplex.a

$(BIN_PERSO_DIR)cca_dist_to_vertex: $(LIB_PERSO_DIR)libcomplex.a


#Les objs
$(OBJ_PERSO_DIR)libinventor.o: $(INC_PERSO_DIR)libcomplex.h $(INC_PERSO_DIR)libcomplex_d.h $(OBJ_PERSO_DIR)libgeneral.o

$(OBJ_PERSO_DIR)libvrml.o: $(INC_PERSO_DIR)libvrml.h $(INC_PERSO_DIR)libinventor_d.h



#Construction des librairies et fichiers o associés...

$(OBJ_PERSO_DIR)libcomplex%.o: $(SRC_PERSO_DIR)lib/libcomplex%.c* $(INC_PERSO_DIR)libcomplex.h
	$(CC) $(CCFLAGS) $< -o $@

$(LIB_PERSO_DIR)libcomplex.a: $(LIB_DPD_LIBCOMPLEX)
	make $(LIB_OBJ_LIBCOMPLEX)
	$(LC1) rcs $@ $(LIB_OBJ_LIBCOMPLEX)

$(LIB_PERSO_DIR)libcomplextotal.a: $(LIB_OBJ_LIBCOMPLEX) $(OBJ_FILES)
	ar rcs $@ $^

#Construction de certains exécutables

$(BIN_PERSO_DIR)raw2pgmP5: $(SRC_PERSO_DIR)com/raw2pgmP5.cxx
	$(CC) -D_FILE_OFFSET_BITS=64 $(LDFLAGS) $< -o $@

$(BIN_PERSO_DIR)shrink_on_disk: $(SRC_PERSO_DIR)com/shrink_on_disk.cxx $(OBJ_PERSO_DIR)libcrop.o
	$(CC) -D_LARGEFILE64_SOURCE=1 -D_FILE_OFFSET_BITS=64 $(LDFLAGS) $^ -L./obj -L$(LIB_PINK) -lpink -lm -o $@

$(BIN_PERSO_DIR)crop_on_disk: $(SRC_PERSO_DIR)com/crop_on_disk.cxx $(OBJ_PERSO_DIR)libcrop.o
	$(CC) -D_LARGEFILE64_SOURCE=1 -D_FILE_OFFSET_BITS=64 $(LDFLAGS) $^ -L./obj -L$(LIB_PINK) -lpink -lm -o $@

$(OBJ_PERSO_DIR)libcrop.o: $(SRC_PERSO_DIR)lib/libcrop.cxx $(INC_PERSO_DIR)libcrop.h
	$(CC) -D_LARGEFILE64_SOURCE=1 -D_FILE_OFFSET_BITS=64 $(CCFLAGS) $< -o $@


#COnstruction générique des exe et des bins

$(OBJ_PERSO_DIR)%.o: $(SRC_PERSO_DIR)lib/%.c* $(INC_PERSO_DIR)%.h
	$(CC) $(CCFLAGS) $< -o $@

$(BIN_PERSO_DIR)%: $(SRC_PERSO_DIR)com/%.cxx
	$(CC) $(LDFLAGS) $< $(ADDLIBS) -o $@


