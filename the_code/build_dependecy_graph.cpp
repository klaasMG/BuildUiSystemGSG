//
// Created by klaas on 2/5/2026.
//

#include <iostream>

int main(int argc, char* argv[]) {
	if (argc == 3)
	{
		char* cpp_file_preprocesor = argv[1];
		char* shader_directory = argv[2];
		std::cout << cpp_file_preprocesor << std::endl;
		std::cout << shader_directory << std::endl;

	}
	else
	{
		std::cout << "wrong number of arguments" << std::endl;
	}

	return 0;
}