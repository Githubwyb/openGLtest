cc = g++ -g -Wall -std=c++11 -lGL -lGLU -lglut -lGLEW
prom = run
deps = $(shell find ./ -name "*.h")
src = $(shell find ./ -name "*.cpp")
obj = $(src:%.cpp=%.o)

$(prom): $(obj)
	$(cc) -o $(prom) $(obj)

%.o: %.cpp $(deps)
	$(cc) -c $< -o $@

clean:
	rm -rf $(obj) $(prom)