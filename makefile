.PHONY : build rebuild test

default :
	@echo "======================================="
	@echo "Please use 'make build' command to build it.."
	@echo "Please use 'make rebuild' command to build it.."
	@echo "Please use 'make test' command to build it.."
	@echo "======================================="

INCLUDES += -I../../../src -I/usr/local/include -I/usr/local/opt/libiconv/include
LIBS = -L../ -L../../ -L../../../ -L/usr/local/lib -L/usr/local/opt/libiconv/lib
# CFLAGS = -Wall -O3 -fPIC --shared -DJEMALLOC -ljemalloc -Wl,-rpath,. -Wl,-rpath,.. -Wl,-rpath,/usr/local/lib
# CFLAGS = -Wall -O3 -fPIC --shared -DTCMALLOC -ltcmalloc -Wl,-rpath,. -Wl,-rpath,.. -Wl,-rpath,/usr/local/lib
CFLAGS = -Wall -O3 -fPIC --shared -Wl,-rpath,. -Wl,-rpath,.. -Wl,-rpath,/usr/local/lib -Wl,-rpath,/usr/local/opt/libiconv/lib

prepare:
	@wget -O libiconv.tar.gz https://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.16.tar.gz && tar zxvf libiconv.tar.gz
	@cd libiconv-* && ./configure --prefix=/usr/local && make && make install
	@rm -rf libiconv*

# 构建liconv.so依赖库
rebuild:
# @$(MAKE) prepare
	@$(CC) -o liconv.so liconv.c $(CFLAGS) $(INCLUDES) $(LIBS) -lcore -llua -liconv
	@mv *.so ../../

# 构建liconv.so依赖库
build:
# @$(MAKE) prepare
	@$(CC) -o liconv.so liconv.c $(CFLAGS) $(INCLUDES) $(LIBS) -lcore -llua -liconv
	@mv *.so ../../

test:
# @$(MAKE) prepare
	@$(CC) -o liconv.so liconv.c $(CFLAGS) $(INCLUDES) $(LIBS) -llua -liconv
