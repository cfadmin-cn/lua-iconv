.PHONY : build rebuild test

default :
	@echo "======================================="
	@echo "Please use 'make build' command to build it.."
	@echo "Please use 'make rebuild' command to build it.."
	@echo "Please use 'make test' command to build it.."
	@echo "======================================="

INCLUDES += -I../ -I../../ -I../../../ -I../../src -I/usr/local/include
LIBS = -L../ -L../../ -L../../../ -L/usr/local/lib

CFLAGS = -Wall -O3 -fPIC --shared -Wl,-rpath,. -Wl,-rpath,.. -Wl,-rpath,/usr/local/lib -Wl,-rpath,/usr/local/opt/libiconv/lib

prepare:
	@wget -O libiconv.tar.gz https://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.17.tar.gz && tar zxvf libiconv.tar.gz
	@cd libiconv-* && ./configure --prefix=/usr/local && make && make install
	@rm -rf libiconv*

# 构建liconv.so依赖库
build:
# @$(MAKE) prepare
	@if [ "`uname -s`" = 'Darwin' ] \
	; then \
		$$@$(CC) -o liconv.so conv.c $(CFLAGS) $(INCLUDES) $(LIBS) -lcore -licucore ; \
	elif [ "`uname -s`" = 'Linux' ] \
	; then \
		$$@$(CC) -o liconv.so conv.c $(CFLAGS) $(INCLUDES) $(LIBS) -lcore ; \
	else \
		$$@$(CC) -o liconv.so conv.c $(CFLAGS) $(INCLUDES) $(LIBS) -lcore -liconv ; \
	fi;
	@mv *.so ../
