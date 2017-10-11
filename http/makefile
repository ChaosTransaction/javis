
###############################################################################
## 编译器
COMPILER = g++
COMPILERC = gcc

## 编译选项
## for debug -DROOM_DEBUG_MCACHE
CPP_FLAG = -g -Wall -w -fPIC -c -D_LINUX -DDEBUG -D_DEBUG -D_REENTRANT -D_GNU_SOURCE -D__DEBUG_CORE__ -DFCGI_STD  #-DCHECK_HEADER-DTEST
RCPP_FLAG = -g -Wall -w -fPIC -c -D_LINUX -DDEBUG -D_DEBUG -D_REENTRANT -D_GNU_SOURCE -DFCGI_STD #-DTEST
## for release
##CPP_FLAG = -O3 -Wall -s -w -fPIC -c -D_LINUX -DNDEBUG -D_REENTRANT -D_GNU_SOURCE -DFCGI_STD

## 链接选项
## for debug
LINK_FLAGS = -g  -fPIC
## for release
## LINK_FLAGS = -s -O3 -shared -fPIC

LIB_DIR = ../../library
BASE_DIR = ./../pub
PLUGIN_DIR = ../plugins
CUSTOM_THIRD_PATH = $(LIB_DIR)/mylib/third_party
## 头文件
INCLUDE_PATHS = -I./\
		-I../\
		-I$(BASE_DIR)/\
		-I$(PLUGIN_DIR)\
		-I$(LIB_DIR)/mylib/public/\
		-I$(LIB_DIR)/mylib/base\
		-I/usr/local/lib\
		-I$(LIB_DIR)/third_party/breakpad/src\
		-I$(CUSTOM_THIRD_PATH)\
		

## 库
LINK_PATHS =  -L./
LINK_LIBRARYS = -lrt -lpthread -lfcgi++ \


# 编译
COMPILE = $(COMPILER) $(CPP_FLAG) $(INCLUDE_PATHS) -o $@
RCOMPILE = $(COMPILER) $(RCPP_FLAG) $(INCLUDE_PATHS) -o $@

# 链接
LINK = $(COMPILER) $(LINK_FLAGS) $(LINK_PATHS) $? -o $(OUTPUT_FILE) $(LINK_LIBRARYS)
LINKALL = $(COMPILER) $(LINK_FLAGS) $(LINK_PATHS) $?  $(LINK_LIBRARYS)

POBJ =  mig_log.o core_interface.o http_formate.o fcgimodule.o  packet_processing.o\
	$(LIB_DIR)/mylib/third_party/dmg_fp/lib/dmg_fp.a\
	$(LIB_DIR)/mylib/base/lib/mybase.a\
	/usr/local/lib/lib/libiconv.a

OUTPUT_FILE = /var/www/cgi-bin/star
#############################################################################################################################
##all: clean proc delete-temp-file
all: proc 

proc: apiproc

######################apiproc###############################################
APINAME = api_
APIPROC_PATH = /api
API_OBJECT_PATH = ./obj$(APIPROC_PATH)
OTJERPROC_VERSION = /v1
apiproc:$(APIPROC_PATH)/dynatick.fcgi

$(APIPROC_PATH)/dynatick.fcgi: $(API_OBJECT_PATH)/$(APINAME)dynatick.o $(POBJ)
	$(LINKALL) -o $(OUTPUT_FILE)$(APIPROC_PATH)$(OTJERPROC_VERSION)/dynatick.fcgi
		
### 编译
$(API_OBJECT_PATH)/$(APINAME)dynatick.o:
	$(COMPILE) .$(APIPROC_PATH)$(OTJERPROC_VERSION)/dynatick/dynatick.cc


clean:
	-rm -f *.o
	-rm -f $(OUTPUT_FILE)./*.fcgi
	-rm -f obj/*.o

delete-temp-file:
	-rm -f obj/*.o

############public###############################



packet_processing.o :
	$(COMPILE) $(BASE_DIR)/net/packet_processing.cc

base_values.o:
	$(COMPILE) $(LIB_DIR)/mylib/base/logic/base_values.cc

http_serializer.o:
	$(COMPILE) $(LIB_DIR)/mylib/base/logic/http_serializer.cc
	
json_serializer.o:
	$(COMPILE) $(LIB_DIR)/mylib/base/logic/json_serializer.cc
	
jsonp_serializer.o:
	$(COMPILE) $(LIB_DIR)/mylib/base/logic/jsonp_serializer.cc
		
logic_comm.o:
	$(COMPILE) $(LIB_DIR)/mylib/base/logic/logic_comm.cc
	
mig_log.o:
	$(COMPILE) $(LIB_DIR)/mylib/public/log/mig_log.cc

basic_util.o:
	$(COMPILE) $(LIB_DIR)/mylib/public/basic/basic_util.cc
	
core_interface.o:
	$(COMPILE) $(LIB_DIR)/mylib/public/comm/core_interface.cc
	
http_formate.o:
	$(COMPILE) $(LIB_DIR)/mylib/public/http/http_formate.cc
	
data_packet.o:
	$(COMPILE) $(LIB_DIR)/mylib/public/protocol/data_packet.cc

fcgimodule.o:
	$(COMPILE) $(BASE_DIR)/fcgimodule/fcgimodule.cc
