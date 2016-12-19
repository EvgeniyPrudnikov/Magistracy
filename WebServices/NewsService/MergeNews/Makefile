all: jsoncpp.cpp MergeNews.cpp
	c++ --std=c++11 jsoncpp.cpp MergeNews.cpp -o ./Release/MergeNews -pthread -O2 \
  -I/usr/include/jsoncpp \
  -I/usr/local/include/mongocxx/v_noabi -I/usr/local/include/libmongoc-1.0 \
  -I/usr/local/include/bsoncxx/v_noabi -I/usr/local/include/libbson-1.0 \
  -L/usr/local/lib -lmongocxx -lbsoncxx