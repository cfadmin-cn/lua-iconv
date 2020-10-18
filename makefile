.PHONY : build rebuild

default :
	@echo "======================================="
	@echo "Please use 'make build' command to build it.."
	@echo "Please use 'make rebuild' command to build it.."
	@echo "======================================="

INCLUDES += -I../../../src -I/usr/local/include
LIBS = -L../ -L../../../ -L/usr/local/lib
# CFLAGS = -Wall -O3 -fPIC --shared -DJEMALLOC -ljemalloc -Wl,-rpath,. -Wl,-rpath,.. -Wl,-rpath,/usr/local/lib
# CFLAGS = -Wall -O3 -fPIC --shared -DTCMALLOC -ltcmalloc -Wl,-rpath,. -Wl,-rpath,.. -Wl,-rpath,/usr/local/lib
CFLAGS = -Wall -O3 -fPIC --shared -Wl,-rpath,. -Wl,-rpath,.. -Wl,-rpath,/usr/local/lib

prepare:
	@wget wget -O libiconv.tar.gz https://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.16.tar.gz && tar zxvf libiconv.tar.gz
	@cd libiconv-* && ./configure --prefix=/usr/local && make && make install
	@rm -rf libiconv*

# 构建libiconv库依赖
rebuild:
# 	@$(MAKE) prepare
	@$(CC) -o liconv.so liconv.c $(CFLAGS) $(INCLUDES) $(LIBS) -lcore -llua -liconv

# 构建liconv.so依赖库
build:
# 	@$(MAKE) prepare
	@$(CC) -o liconv.so liconv.c $(CFLAGS) $(INCLUDES) $(LIBS) -lcore -llua -liconv