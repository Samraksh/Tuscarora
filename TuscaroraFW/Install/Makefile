
SHELL := /bin/bash

# create directory to hold the dummy TARGET files (t)
DUMMY := $(shell mkdir -p t)

#With every update to this file the following version should be changed. 
TUSC_VERSION:=2.0
# installation makefile: All the steps necessary to build a complete
# See http://bost.ocks.org/mike/make/
# SHELL=/bin/bash
#DCE_VERSION=dce-linux-1.2
DCE_VERSION:=dce-ns3-1.7
DCE_DIR:=${HOME}/dce
#DCE_BUILD_METHOD=waf
DCE_BUILD_METHOD:=bake
BAKE_HOME:=${HOME}/bake
CURPATH=$(shell pwd)
TUS := ${CURPATH}/..
#HOME=$(shell cd && pwd)
SIMRESULTS := ${HOME}/simresults
BASHRC:=${HOME}/.bashrc
XPROFILE:=${HOME}/.xprofile

NS3_DCE:=${DCE_DIR}/source/ns-3-dce
DCE_MODEL:=${NS3_DCE}/model
# NS3=$(HOME)/dce/source/ns-3.20
DCE_RAW_DIR:=${NS3_DCE}/raw

# These variables depend on ns3 being installed (t/install-ns3)
# because they point to directories in the ns3 installation:
NS3_VER := $(shell [[ -d "$(DCE_DIR)" ]] && cd $(DCE_DIR) && ${BAKE_HOME}/bake.py show | \
	perl -lne 'if (!$$done and /module: (ns-3.\S+) \(enabled\)/) { print $$1; $$done=1;}')
ifdef NS3_VER
NS3_DIR := ${HOME}/dce/source/${NS3_VER}
endif

# Patching variables:
dcePatchSrc := $(TUS)/Patchs
ns3PatchSrc := $(TUS)/Patchs.ns3
#DCE_PATCHES := $(shell cd $(dcePatchSrc); find . -name '*.patch')
#NS3_PATCHES := $(shell cd $(ns3PatchSrc); find . -name '*.patch')



# All targets which generate files in the Install dir should start
# with t/ to avoid polluting.

# This variable stores all the ubuntu packages necessary (or useful)
# to get dce and Tuscarora installed. These are turned into
# targets which get built the same way.
apt-get-install=t/ack-grep t/bison t/bzr t/cmake t/ddd \
		t/flex t/git-core t/libc6-dbg t/build-essential \
		t/libdb-dev t/libexpat1-dev t/libpcap-dev \
		t/libqt4-dev t/mercurial t/parallel \
		t/perl-doc t/pkg-config t/python-dev t/python-setuptools \
		t/python-pygoocanvas t/python-pygraphviz t/qt4-dev-tools \
		t/valgrind t/vim \
		#t/protobuf-compiler t/python-protobuf t/libboost-dev t/libxml2-dev
		#t/openmpi-bin t/openmpi-common  t/libopenmpi-dev \
		#t/libglew-dev t/libcheese* t/libclutter-gst-* t/libcogl* t/libclutter-gtk-1.0-0 t/libclutter-1.0-0 \

#t/python-dev t/python-pygccxml t/gccxml t/libboost-dev\
#t/build-essential \

perl-modules=t/Modern__Perl

# targets in .PHONY always get rebuilt. We don't use too many .PHONYs
# because we generate files to avoid rebuilding unnecessarily.
.PHONY: all query-user # reset-install-dce

# all is the default target
all: t/install-fw
	@echo $@ Done

# A Framwork install just connects the fw files to dce with environmental variables, patches, and symlinks
#t/install-fw: t/ubuntu-pkgs $(perl-modules) t/config-limits t/install-dce t/set-env t/symlink-fw t/patch-fw
t/install-fw: t/ubuntu-pkgs $(perl-modules) t/install-dce t/set-env t/symlink-fw t/patch-fw
	touch $@
	@echo "$@: connected to dce"

t/ubuntu-pkgs: $(apt-get-install)
	#touch $@
	#sudo rm /usr/bin/gcc; sudo ln -s /usr/bin/gcc-4.9 /usr/bin/gcc
	#sudo rm /usr/bin/g++; sudo ln -s /usr/bin/g++-4.9 /usr/bin/g++
	@echo "$@ done"

t/waveform:
	ln -s ${TUS}/ns-3.x/src/tuscarora-wireless ${NS3_DIR}/src/tuscarora-wireless
	@echo "Linked tuscarora-wireless to ns3"
	touch $@
	
# Install environmental variables by adding to .bashrc
# In a clean environment (env -i),
# run an intercative shell (bash -i), which sources .bashrc,

# with the command (bash -c) testing if variable is set.
# If the variable is not set, prepend it to .bashrc.
setIfNotSet =	if env -i bash -ci '[[ -z "$$$1" ]]'; then \
		  echo installing ${$1}; echo "Printing $$$1"; \
		  echo "$$(echo export $1=${$1} | cat -  $(XPROFILE))" >  $(XPROFILE); \
		else echo $1 exists; fi

t/set-env: t/install-dce
	@echo $@
	@$(call setIfNotSet,TUS)
	@echo ${NS3_DCE}
	@$(call setIfNotSet,NS3_DCE)
	@$(call setIfNotSet,DCE_DIR)
	@$(call setIfNotSet,DCE_RAW_DIR)
	@$(call setIfNotSet,BAKE_HOME)
	@$(call setIfNotSet,NS3_DIR)
	touch $@
	@echo $@ done
	
t/mercurial-username: 

	touch $@
	@echo $@ done
	
t/install-dce-check-previous-installation: 
	if (cd ${DCE_DIR} && [[ -d source ]] && [[ -n "$(NS3_VER)" ]] && [[ -d "$(NS3_DIR)" ]] && [[ -d "$(NS3_DCE)" ]] && cd ${NS3_DIR} ); then \
		./checkPrevTuscInstall.sh ${TUSC_VERSION} ${NS3_DCE} ${NS3_DIR}; \
	else \
		echo "$@ source files are not found";\
	fi
	@echo $@ done


# The DCE install depends on the version. 
t/install-dce: t/install-dce-check-previous-installation t/installed-dce-version_$(DCE_VERSION) 
	@echo $@: dce_dir2=$(DCE_DIR)
	# Reset these variables here because they may have changed.
	$(eval NS3_VER := $(shell cd "$(DCE_DIR)" && ${BAKE_HOME}/bake.py show | \
	                    perl -lne 'if (!$$done and /module: (ns-3.\S+) \(enabled\)/) { print $$1; $$done=1;}'))
	$(eval NS3_DIR := ${DCE_DIR}/source/${NS3_VER})
	[[ -n "$(NS3_VER)" ]] && [[ -d "$(NS3_DIR)" ]]
	#Echo and mark as installed
	@echo "dce installed"
	touch $@
	@echo $@ done

# DCE Installation:
# http://www.nsnam.org/docs/dce/manual/html/getting-started.html
#t/installed-dce-version_$(DCE_VERSION): t/install-bake t/dce-dependencies t/dce-distributed-dependencies $(DCE_DIR)
t/installed-dce-version_$(DCE_VERSION): t/install-bake t/dce-dependencies $(DCE_DIR)
	@echo making $@ because of $?
	if (cd ${DCE_DIR} && [[ -d source ]] && \
	     ${BAKE_HOME}/bake.py show --enabled | \
	       grep "module: $(DCE_VERSION) (enabled)" >& /dev/null && \
	    [[ -n "$(NS3_VER)" ]] && [[ -d "$(NS3_DIR)" ]] && cd ${NS3_DIR} && \
	      ./waf install > $(TUS)/Install/t/wafInstallDCE \
	                   2> $(TUS)/Install/t/wafInstallDCE.stderr ); \
	then \
	  echo "$@ is already made, in $$(pwd)"; \
	  [[ -f "$@" ]] || touch "$@"; \
	else \
	  echo baking dce; \
	  mkdir -p ${DCE_DIR}; \
	  ( cd ${DCE_DIR}; \
	    ${BAKE_HOME}/bake.py configure -e ${DCE_VERSION} && echo configured && \
	    ${BAKE_HOME}/bake.py download | tee $(TUS)/Install/t/bakeDownloadDCE && echo downloaded && \
	    ${BAKE_HOME}/bake.py build -vvv | tee $(TUS)/Install/t/bakeBuildDCE ); \
	    if grep -q "Error:  Critical dependency" $(TUS)/Install/t/bakeBuildDCE ; then exit 1; fi; \
	    echo built && ${MAKE} create-mercurial-branch;  \
	  touch "$@"; \
	fi
	[[ -d $(DCE_DIR)/source ]]
	[[ -f $@ ]]
	@echo $@ OK
	
test:
	${MAKE} create-mercurial-branch \

create-mercurial-branch:
	./createMercBranch.sh ${TUSC_VERSION} ${NS3_DCE} ${NS3_DIR}; 
	
# Avoid sudo by testing for a package before trying to install it
# (installation requires sudo).
${apt-get-install}:
	t=$(patsubst t/%,%,$@); \
	if dpkg --get-selections | grep -qE "^$$t.*\s+install"; then \
	  echo "package $@ is installed. "; \
	else \
	  echo "package $@ is missing. Installing . . ."; \
	  sudo apt-get -y install $$t; \
	fi
	touch $@
	@echo $@ done

$(perl-modules):
	@echo installing perl module $@
	# cpan $$(echo "$@" | perl -pe 's:^t/::; s/__/::/;') # not working
	touch $@
	@echo installed perl module $@

t/config-limits:
	@echo $@: checking limits on number of files and processes.
	@maxFiles=8096; \
	if (( `ulimit -n` < $$maxFiles )); then \
	  echo "increasing the max number of files from $$(ulimit -n) to $$maxFiles"; \
	  echo -e "* hard nofile $$maxFiles\n* soft nofile $$maxFiles" | \
	  sudo tee -a /etc/security/limits.conf; \
	fi;
	@maxProcs=3500; \
	if (( `ulimit -u` < $$maxProcs )); then \
	  echo "increasing the max number of files from $$(ulimit -u) to $$maxProcs"; \
	  echo -e "* hard nproc $$maxProcs" | \
	  sudo tee -a /etc/security/limits.conf; \
	fi;
	touch $@
	@echo $@ done

t/install-bake: t/mercurial | ${HOME}/bake
	@echo $@
	hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} pull -R ${HOME}/bake -u
	@echo "Bake is up-to-date."
	#if [[ -z "$$(which bake.py)" ]]; then \
	#   echo "bake.py not in the path: $$PATH"; false; \
	#fi
	touch $@
	@echo $@ done

${HOME}/bake: | t/mercurial
	@echo $@h
	[[ -d "$@" ]] || hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} clone http://code.nsnam.org/bake ${HOME}/bake

# These packages are handled by the apt-get-install rule.
t/dce-dependencies: t/bzr t/cmake t/mercurial t/python-dev \
		t/python-pygoocanvas t/python-pygraphviz t/libpcap-dev \
		t/libexpat1-dev t/libqt4-dev t/pkg-config t/libdb-dev \
		t/bison t/flex t/libc6-dbg t/qt4-dev-tools t/vim \
		t/valgrind t/git-core t/install-bake 
	touch $@
	@echo $@ done

# DCE/NS3 Parallel simulation:
# ----------------------------
# - Dependencies: 
t/dce-distributed-dependencies: t/libopenmpi-dev t/openmpi-common t/openmpi-bin
	touch $@
	@echo $@ done

$(DCE_DIR): 
	mkdir -p ${DCE_DIR}


# Running TuscTest under ns3-dce
# ------------------------------------------

# - Create a symlink of ns-3-dce/myscripts/TuscTest directory under your ns3-dce instalation.

t/symlink-fw: $(NS3_DCE)/myscripts/TuscTest ${TUS}/dceln ${TUS}/simresults
	touch $@
	@echo $@ done

$(NS3_DCE)/myscripts/TuscTest: t/install-dce
	@echo "Installing $@ symlink."
	if [[ -h "$@" ]]; then rm "$@"; fi
	ln -fs "${TUS}/ns-3-dce/myscripts/TuscTest" "$@"
	touch $@
	@echo made $@

$(TUS)/dceln: t/install-dce
	@echo "Installing $@ symlink to output."
	if [[ -h "$@" ]]; then rm "$@"; fi; \
	ln -fs "$(NS3_DCE)" "$@"; \
	[[ -h "$@" ]] # check if target was made
	touch $@
	@echo made $@

$(TUS)/simresults: t/install-dce
	@echo "Installing $@ symlink to output."
	mkdir -p "$(SIMRESULTS)"
	l="$(NS3_DCE)/simresults"; \
	if [[ -h "$$l" ]]; then rm "$$l"; fi; \
	if [[ -h "$@" ]]; then rm "$@"; fi; \
	ln -fs "$(SIMRESULTS)" "$$l"; ln -fs "$$l" "$@"
	[[ -h "$@" ]] # check if target was made
	cd $
	touch $@
	@echo made $@

# patch.d is the dependency file which stores the rules needed to
# apply the patches.  After patches they are copied to the directory
# where they are applied to. Make checks to see if they are there.
# t/patch.d : $(DCE_PATCHES) $(NS3_PATCHES) writePatch.pl safepatch.sh | t/install-dce 
# 	@echo Making patches.d
# 	echo "# Patching" > "$@"
# 	[[ -n "$(NS3_DCE)" ]] && [[ -d "$(NS3_DCE)" ]] # test for destination directory
# 	$(eval dcePatchDest := $(NS3_DCE))
# 	./writePatch.pl t/patch-dce "$(dcePatchSrc)" "$(dcePatchDest)" $(DCE_PATCHES) >> "$@"
# 	[[ -n "$NS3_DIR" ]] && [[ -n "$NS3_DIR/src" ]] # test for destination directory
# 	$(eval ns3PatchDest := $(NS3_DIR)/src)
# 	./writePatch.pl t/patch-ns3 "$(ns3PatchSrc)" "$(ns3PatchDest)" $(NS3_PATCHES) >> "$@"
# 	@echo "$@ rebuilt"

#include t/patch.d

t/patch.d : installPatches.sh safepatch.sh t/ns3-radios | t/install-dce 
	./installPatches.sh ${TUS} ${DCE_DIR} ${NS3_DIR}
	#cd ${DCE_DIR}; ${BAKE_HOME}/bake.py build -vvv | tee $(TUS)/Install/t/bakeBuildDCE && echo built; 
	touch $@
	@echo "$@ done"

t/ns3-radios:
	@echo "Installing $@ symlink."
	if [[ -h "${NS3_DIR}/src/simple-wireless" ]]; then rm "${NS3_DIR}/src/simple-wireless"; fi
	#ln -s "${TUS}/ns-3.x/src/simple-wireless" "${NS3_DIR}/src/"
	
	if [[ -h "${NS3_DIR}/src/slim" ]]; then rm -f "${NS3_DIR}/src/slim"; rm -f ${NS3_DIR}/scratch/slim*.cc; rm -f ${NS3_DIR}/scratch/*.h;  fi
	ln -s "${TUS}/ns-3.x/src/slim" "${NS3_DIR}/src/"
	ln -s ${TUS}/ns-3.x/scratch/slim*.cc ${NS3_DIR}/scratch/
	ln -s ${TUS}/ns-3.x/scratch/*.h ${NS3_DIR}/scratch/
	
	#if [[ -h "${NS3_DIR}/scratch/slim-example.cc" ]]; then rm "${NS3_DIR}/scratch/slim-example.cc"; fi
	#ln -s "${TUS}/ns-3.x/scratch/slim-example.cc" "${NS3_DIR}/scratch/"
	#cd ${NS3_DIR}; ./waf clean; ./waf install
	#cd ${DCE_DIR};  ${BAKE_HOME}/bake.py build -vvv | tee $(TUS)/Install/t/bakeBuildDCE && echo built; 
	touch $@
	@echo made $@

# The dependencies for .patch-ns3 and .patch-dce are handled in ./writePatch.pl.
t/patch-ns3 t/patch-dce:
	touch "$@"
	@echo "$@ done"


t/patch-fw: t/patched-version_${DCE_VERSION}
	touch $@
	@echo $@: Framework is patched

t/patched-version_${DCE_VERSION}: t/install-dce t/patch.d t/patch-ns3 t/patch-dce
	./createCommmitForInstall.sh ${TUSC_VERSION} ${NS3_DCE} ${NS3_DIR}
	rm -f t/patched-version*
	( cd ${DCE_DIR} && ${BAKE_HOME}/bake.py build -vvv | tee $(TUS)/Install/t/bakeBuildPatchedDCE)
	if grep -q "Error:  Critical dependency" $(TUS)/Install/t/bakeBuildPatchedDCE ; then exit 1; fi
	touch $@
	@echo made $@


patch-fw: t/patch-fw
	@echo Framework patched.

unpatch: QUERY := This is experimental and could break your installation. Are you sure?
unpatch: query-user
	if [[ -n "$(NS3_DCE)" ]] && [[ -d "$(NS3_DCE)" ]] && \
	   [[ -n "$NS3_DIR" ]] && [[ -n "$NS3_DIR/src" ]]; then \
	  dcePatchDest="$(NS3_DCE)"; \
	  ns3PatchDest="$(NS3_DIR)/src"; \
	  appliedPatches="`find "$$dcePatchDest" -name '*.patch'`"; \
	  appliedPatches+=" `find "$$ns3PatchDest" -name '*.patch'`"; \
	  echo "appliedPatches = $$appliedPatches"; \
	  for p in $$appliedPatches; do \
	    echo -e "$@: Attempting to unpatch $$p . . ."; \
	    if patch --dry-run -d `dirname "$$p"` -stR < "$$p"; then \
	      echo -e "$@: unpatching $$p\n"; \
	      patch -d `dirname "$$p"` -stR < "$$p" && rm "$$p"; \
	    else echo -e "$@: could not unpatch $$p\n"; fi \
	  done; \
	else echo "rm-patches: cannot find patch destination directory."; fi
	@echo "removed applied patches: $+"

remove-bake: 
	# Remove bake by removing it's directory
	[ -e ${HOME}/bake ] && rm -rf ${HOME}/bake


# Removing dce is done by removing its directory
remove-dce: 
	@echo $@
	rm -rf "${DCE_DIR}" t/*
	@echo $@: removed dce

query-user:
	@while true; do \
	  read -p "$(QUERY) [n] " yn; \
	  case $$yn in \
	    [Yy]* ) echo proceeding; break ;; \
	    * ) echo exiting; exit 1 ;; \
	  esac ; \
	done

rebuild-dce rebuild-fw: QUERY := This will remove the entire dce folder. Are you sure?
rebuild-dce rebuild-fw: query-user remove-dce t/install-fw
	@echo dce rebuilt


t/dce.test: t/install-dce t/dce-udp-simple.test t/dce-iperf.test
	@echo $@ OK

t/dce-udp-simple.test:
	@echo $@
	cd ${NS3_DCE}; ./waf --run dce-udp-simple; \
	[[ -e "elf-cache" ]] && [[ -e "files-1" ]] && [[ -e "exitprocs" ]]
	@echo $@ OK

t/dce-iperf.test:
	@echo $@
	cd ${NS3_DCE}; ./waf --run dce-iperf
	@echo $@ OK





t/fw.test: t/fw-gossip.test
	@echo "$@ OK"

t/fw-gossip.test: t/install-dce t/install-fw
	@echo $@
	cd ${TUS}; ./runOrDebug.sh -c -p dce Gossip
	@echo "$@ OK"


GIT_PROMPT_DESC=git prompt: see https://github.com/DanielAtSamraksh/bash-git-prompt-1
GIT_PROMPT=${HOME}/.bash-git-prompt
t/git-prompt: 
	@echo $@
	# https://github.com/DanielAtSamraksh/bash-git-prompt-1
	[ -e "${GIT_PROMPT}" ] || git clone git@github.com:DanielAtSamraksh/bash-git-prompt-1.git "${GIT_PROMPT}"
	grep -q "bash-git-prompt" "${XPROFILE}" || echo "# ${GIT_PROMPT_DESC}\nsource ${GIT_PROMPT}/gitprompt.sh" >> "${XPROFILE}"
	touch $@
	@echo $@ done

t/uninstall: t/uninstall.ns3 t/uninstall.dce
	# This method reverts changes made by the Tuscarora system and rebuilds dce to have it without the patches
	# installed by the Tuscarora system
	@echo $@
	#  ( cd ${DCE_DIR}; \
	#    ${BAKE_HOME}/bake.py configure -e ${DCE_VERSION} && echo configured && \
	#    ${BAKE_HOME}/bake.py build -vvv | tee $(TUS)/Install/t/bakeBuildDCE_postuninstall && echo built; ); \
	    
t/uninstall.dce:	
	@echo $@
	(	cd ${NS3_DCE}  && \
		hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} sta && \
		hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} up -C && \
		hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} st -un0 | xargs -0 rm && \
		hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} sta; );
	
t/uninstall.ns3:
	@echo $@
	(	cd ${NS3_DIR}  && \
		hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} sta && \
		hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} up -C && \
		hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} st -un0 | xargs -0 rm && \
		hg --config ui.username=TuscaroraInstaller-${TUSC_VERSION} sta; ); 
		#&& \
		#./waf configure --enable-examples -d optimized --prefix=$HOME/dce/build --includedir=$HOME/dce/include/${NS3_VER}; );
		
