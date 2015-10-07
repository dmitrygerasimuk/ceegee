# Copyright (C) 2015, Michiel Sikma <michiel@sikma.org>
# MIT License

CC        = $(DJGPP_CC)
VENDOR    = vendor
CFLAGS    = -Ivendor/allegro/include
LDFLAGS   =

TITLE     = CeeGee Engine
BIN       = ceegee.exe
SRCDIR    = src
OBJDIR    = obj
DISTDIR   = dist
STATICDIR = src/static

# Static files, e.g. the readme.txt file, that get copied straight to
# the dist directory.
STATIC    = $(shell find $(STATICDIR) -name "*.*")
STATICDEST= $(subst $(STATICDIR),$(DISTDIR),$(STATIC))

# All source files (*.c) and their corresponding object files.
SOURCES   = $(shell find $(SRCDIR) -name "*.c" -not -path "$(VENDOR)/allegro/*") \
            $(shell find $(VENDOR) -name "*.c" -not -name "test_*.c" -not -path "$(VENDOR)/allegro/*") \
            $(shell find "$(VENDOR)/allegro/src" -name "*.c")
OBJS      = $(SOURCES:%.c=%.o)

# Some information from Git that we'll use for the version indicator file.
HASH      = $(shell git rev-parse HEAD)
BRANCH    = $(shell git describe --all | sed s@heads/@@ | awk "{print toupper($0)}")
COUNT     = $(shell git rev-list HEAD --count)
DATE      = $(shell date +"%Y-%m-%d %T")

.PHONY: clean dir version
default: all

check_djgpp:
	@if [ -z "$$DJGPP_CC" ]; then \
        echo "To compile, you'll need to set the DJGPP_CC environment variable to a DJGPP GCC binary, e.g. /usr/local/djgpp/bin/i586-pc-msdosdjgpp-gcc"; \
        exit 2; \
	fi

dir:
	@mkdir -p ${DISTDIR}

version:
	@echo "${TITLE}\nBuild: ${COUNT}-${BRANCH} ${DATE}\nHash: ${HASH}" > ${DISTDIR}/version.txt

%.o: %.c | check_djgpp
	${CC} -c -o $@ $? ${CFLAGS}

${DISTDIR}/${BIN}: ${OBJS} | check_djgpp
	${CC} -o ${DISTDIR}/${BIN} $+ ${LDFLAGS}

${STATICDEST}: ${STATIC}
	cp $< $@

all: dir version ${DISTDIR}/${BIN} ${STATICDEST}

clean:
	rm -rf ${DISTDIR}
	rm -f ${OBJS}
