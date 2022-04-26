#include "Header.hpp"
#include "Axes.hpp"
#include "Vector3D.hpp"
#include "Line3D.hpp"
#include "Plane3D.hpp"
#include "Vector2D.hpp"
#include "HashTable.hpp"
#include "Graph.hpp"
#include "mattress.hpp"

#include <fstream>

unsigned int HashFunction(std::string key){
	unsigned long size = key.size();
	unsigned int ret = 0;
	for(int i=0; i<size; i++) ret += (char)key[i] * (char)key[i];
	return ret*ret;
}

bool EqualityFunction(std::string a, std::string b){
	return (a == b && a.size() == b.size());
}

int main() {
	
	HashTable<std::string, 10000> hashTable = HashTable<std::string, 10000>(&HashFunction, &EqualityFunction);
	
	std::fstream file;
	file.open("/Users/Edzen098/Projects/Tools2/uk-towns-sample.csv");
	std::string record;
	std::getline(file, record);
	while(true){
		if(!std::getline(file, record, ',')) break;
		if(!std::getline(file, record, ',')) break;
		hashTable.Add(record);
		if(!std::getline(file, record)) break;
	}
	file.close();
	
	hashTable.Print();
	
	std::cout << "Test: " << hashTable.Find("Aspley") << ", " << hashTable.Find("Achahoish") << ", " << hashTable.Find("Edmund") << std::endl;
	
	std::cout << hashTable[100] << std::endl;
	
	std::cout << hashTable.ItemNumber() << std::endl;
	hashTable.Remove("Ardington");
	
	hashTable.Add("Ashchurch");
	
	hashTable.Print();
	
	return 0;
}
