all: NewsService.cpp NewsServiceHandler.cpp NSMongoStorage.cpp 
	c++ --std=c++11 NewsService.cpp NSMongoStorage.cpp NewsServiceHandler.cpp -shared -o ./Release/libNewsService.so -lfastcgi-daemon2 -fPIC -pthread -O2 \
  -I/usr/local/include/mongocxx/v_noabi -I/usr/local/include/libmongoc-1.0 \
  -I/usr/local/include/bsoncxx/v_noabi -I/usr/local/include/libbson-1.0 \
  -L/usr/local/lib -lmongocxx -lbsoncxx
		