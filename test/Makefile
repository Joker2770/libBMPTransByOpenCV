cc = g++
prom = testlibBMPTrans
src = $(shell find ./ -name "*.cpp")
obj = $(src:%.cpp=%.o) 

$(prom): $(obj)
	$(cc) -ldl -o $(prom) $(obj)

%.o: %.cpp
	$(cc) -o $@ -c $<

clean:
	rm -rf $(obj) $(prom)
