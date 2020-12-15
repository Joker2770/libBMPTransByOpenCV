cc = g++
prom = testlibBMPTrans
src = $(shell find ./ -name "*.cpp")
obj = $(src:%.cpp=%.o) 

$(prom): $(obj)
	$(cc) -o $(prom) $(obj) -ldl

%.o: %.cpp
	$(cc) -o $@ -c $<

clean:
	rm -rf $(obj) $(prom)
