# Wides_2023
1. copy the "python_api_test.h" file to the same directory your cpp file exists
2. add #include "python_api_test.h" in your cpp file
3. call function python_api(func, args) to run python scripts
4. "func" is the name of the python script and "args" are the parameters which should be used by the python script.
5. "func" is the combination of path and name of python scripts. eg: if the python script whose name is "test_3" and located at ./python_code/, then "func" should be "./python_code/test_3".
6. "args" example: "10 20 30" means passing 3 arguments "10", "20", and "30" to the python script; "Wides 2023" means passing 2 arguments, "Wides" and "2023", to the python scripts  
