#include "ESDL_General.hpp"

// I took this function from somewhere on the internet a long time ago and have adapted it since
char* filetobuf(const char *const &file){
	FILE *fptr;
	long length;
	char *buf;
	
	fptr = fopen(file, "rb"); // Open file for reading
	if(!fptr){
		std::cout << "ERROR: File " << file << " not found." << std::endl;
		return NULL;
	}
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file
	length = ftell(fptr); // Find out how many bytes into the file we are
	buf = (char *)malloc(length+1); // Allocate a buffer for the entire length of the file and a null terminator
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer
	fclose(fptr); // Close the file
	buf[length] = 0; // Null terminator

	return buf;
}
