#include <fstream>
#include <sstream>
#include "transaction.h"
#include <iterator>
#include <iostream>
#include <stdlib.h>

//Declaration definitions
transaction::transaction()
{

}

void transaction::getTransactions(std::string file)
{
  std::string line;
  std::ifstream infile(file.c_str());
  std::vector<std::string>::iterator it;
  while(std::getline(infile,line))
    {
      //Parse by whitespace, store words in vector
      std::stringstream ss(line);
      std::istream_iterator<std::string> begin(ss);
      std::istream_iterator<std::string> end;
      std::vector<std::string> words(begin,end);
      //Take each position in the array store words    

      if(words[0] == "FoodItem")
	{
	  std::string name;
	  for (int count = 9; words[count] != words.back(); count++)
	    {
	      name = name + words[count] + " ";
	    }
	  name = name + words.back();
	  char *y = const_cast<char *>(words[7].c_str());
	  int shelflife = atoi(y);
	  food_item food(words[4], name,shelflife,0);
	  foodList.push_back(food);
	  std::cout << food.getUPC() + " " << food.getName() + " " << std::endl;
	}     
 
      else if(words[0] == "Warehouse")
	{
	  //add new warehouses
	  warehouse ware(words[2]);
	  warehouseMap.insert(std::pair<std::string, warehouse>(words[2], ware));
	}
     
      
      /* for(it=words.begin(); it != words.end(); it++)
	 {
	   std::cout << *it << std::endl;
	 }
       std::cout << std::endl;
      */
       
    }
      typedef std::map< std::string, warehouse >::iterator outer_iterator;
      for(outer_iterator outer = warehouseMap.begin() ; outer != warehouseMap.end() ; ++outer )
	{
	  std::cout << outer->first << std::endl;
	}
      std::cout << warehouseMap.size() << std::endl;

}

