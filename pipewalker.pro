SUBDIRS  += src
TEMPLATE  = subdirs 
CONFIG   += warn_on \
	    thread \
            qt

bintarget.path   = /usr/bin
bintarget.files  = ./bin/pipewalker
INSTALLS        += bintarget
