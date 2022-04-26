#include "Header.hpp"
#include "Axes.hpp"
#include "Vector3D.hpp"
#include "Line3D.hpp"
#include "Plane3D.hpp"
#include "Vector2D.hpp"
#include "HashTable.hpp"
#include "Graph.hpp"
#include "mattress.hpp"

LinkedList<float> list;

int main() {
	
	list = LinkedList<float>();
	
	list.Append(0.5f);
	list.Append(1.0f);
	list.Prepend(0.25f);
	list.Append(2.0f);
	list.Prepend(0.125f);
	
	list.Print();
	
	std::cout << list.Remove(1) << ", " << list.Pop() << std::endl;
	
	std::cout << list[2] << ", " << list[1] << ", " << list[0] << std::endl;
	
	std::cout << list.Size() << std::endl;
	
	for(float value = list.Loop(); list.End(); value = list.Next()){
		std::cout << value << " ";
	}
	std::cout << std::endl;
	
	{
		LinkedList<float> list2 = LinkedList<float>();
	
		list2.Append(20.0f);
		list2.Append(40.0f);
		
		list.Append(list2);
	}
	
	list.Print();
	
	{
		LinkedList<float> list2 = list.Reversed();
	
		list2.Print();
		
		list2.Destroy();
	}
	
	list.Destroy();
	
	std::cout << list.Size() << std::endl;
	
	list.Print();
	
	list = MakeLinkedList(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
	
	list.Print();
	
	return 0;
}
