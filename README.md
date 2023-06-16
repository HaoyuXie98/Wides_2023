# Wides_2023
"python_api_test.h" could help to run python script in c++ file. It can run python with any number of parameters at any location and read python's running results by reading and writing txt files.

Usage:
1. copy the "python_api_test.h" file to the same directory your cpp file exists
2. add below head files in your c++ file

              #include "python_api_test.h“
              #include <map>
              #include <string> 


5. use function python_api(func, args) to run python scripts, eg:
   
          std::map<std::string, std::string> ret = python_api(name, args);
   
7. "name" is the combination of path and name of the python script, eg: if the python script whose name is "test_3" and located at ./python_code/, then "name" should be "./python_code/test_3".
8. "args" are the parameters which should be passed to the python script.
9. "func" is the combination of path and name of python scripts, eg: "10 20 30" means passing 3 arguments "10", "20", and "30" to the python script; "Wides 2023" means passing 2 arguments, "Wides" and "2023", to the python scripts  
10. In your python script, use below function to write txt file to send return value to your c++ file. "data" is dicitonary
   
              def writetxt(data):
                with open("output.txt", "a") as f:
                  for key, value in data.items():
                    f.write(f"{key},{value}\n")
   
   eg:
   
                data = {
                     "name": args.name,
                     "age": args.age
                }

         writetxt(data)
11. To show the Python script exited status, you can add "sys.exit()" in your Python script. eg: sys.exit(123) will print 123 in your terminal.
