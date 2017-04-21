#include <fstream>
#include <sstream>
#include "transaction.h"
#include <iterator>
#include <iostream>
#include <stdlib.h>
#include <set>

/*
 * Assignment 4 2/17/2017
 * Sean Hammond and Michael Sorger
 * This class takes a text file of transactions with Food items 
 * warehouses, request, received, day increments. This class uses helper classes
 * food_item and warehouse. 
 */
//Declaration definitions

transaction::transaction()
{
}

void transaction::getTransactions(std::string file)
{
  std::string line;
  std::ifstream infile(file.c_str());
  std::vector<std::string>::iterator it;
  std::map<std::string, int> requestCount; // Used to calculate the most popular items
  while(std::getline(infile,line))
    {
      //Parse by whitespace, store words in vector
      std::stringstream ss(line);
      std::istream_iterator<std::string> begin(ss);
      std::istream_iterator<std::string> end;
      std::vector<std::string> words(begin,end);    
     

      //Parse first word in vector, could be FoodItem, Warehouse, Receive:, Request:, Next Day, End

      //First word is FoodItem
      if(words[0] == "FoodItem")
	{
	  std::string name;
	  // In the text file it the line starts with food item we know the 9th position of the line is the start of the name
	  for (int count = 9; words[count] != words.back(); count++)
	    {
	      name = name + words[count] + " ";
	    }
	  name = name + words.back();
	  char *y = const_cast<char *>(words[7].c_str()); //changes string to character array
	  int shelflife = atoi(y); //converts character array to an int
	  food_item food(words[4], name,shelflife,0,0);
	  // inserting the new food item into the list
	  foodList.insert(std::pair<std::string,food_item>(words[4],food));
	  // adding the food item into the request counter
	  requestCount.insert(std::pair<std::string,int>(words[4],0));
	}     
 
      //First word is Warehouse
      else if(words[0] == "Warehouse")
	{
	  //add new warehouses
	  std::string name;
	  // if the line starts with warehouse we know the position 2 is the start of the name
	  for( int i = 2; i < words.size(); i ++)
	    {
	      name = name + words[i] + " ";
	    }
	  // removes the new line character from the name
	  name.erase(name.size() - 1);
	  warehouse ware(name);
	  warehouseMap.insert(std::pair<std::string, warehouse>(name, ware));
	}
     
      //First word is Receive:
       else if(words[0] == "Receive:")
	{
	  food_item addFood = foodList[words[1]];
	  char *y = const_cast<char *>(words[2].c_str()); //changes string to character array
	  int quantity = atoi(y); //stores quantity of items received
	  addFood.changeQuantity(quantity);
	  addFood.setDate(this->maindate);

	  std::string name;
	  // if the line started with receive we know the name position started on 3
	  for(int i = 3; i < words.size(); i++)
	    {
	      name = name + words[i] + " ";
	    }
	  // removed new line character from the name
	  name.erase(name.size() - 1);
	  warehouseMap[name].receive(addFood);

	  int change = foodList[words[1]].getTotal();
	  change = change + quantity;
	  foodList[words[1]].changeQuantity(change);
	}
      
      //First word is Request
       else if(words[0] == "Request:")
	 {
	  
	   // we know the UPC code was it position 1 of the line
	  food_item subFood = foodList[words[1]];
	  char *y = const_cast<char *>(words[2].c_str()); //changes string to character array
	  int quantity = atoi(y); //stores quantity of items received
	  subFood.changeQuantity(quantity);
	  
	  std::string name;
	  // if the line started with request we know the name position started on 3
	  for(int i = 3; i <  words.size(); i++)
	    {
	      name = name + words[i] + " ";
	    };
	  name.erase(name.size() - 1); // removed the new line character from the name
	  std::map<std::string, warehouse>::iterator wareit = warehouseMap.find(name);
	  wareit->second.request(subFood);
	  // calls the request method inside the warehouse
	  std::map<std::string,int>::iterator reqit = requestCount.find(subFood.getUPC());
	  reqit->second = reqit->second + 1;
	 }

      //first words are next day
       else if(words[0] == "Next")
	 {
	   maindate = maindate + 1;
	   std::map<std::string, warehouse>::iterator it;
	   for(it = warehouseMap.begin(); it != warehouseMap.end(); it++)
	     {
	       it->second.update(maindate);
	     }
	 }
       else if(words[0] == "Start")
	 {
	   maindate = 0;
	   continue;
	 }
      // if End stop checking the text file
       else if(words[0] == "End")
	 {
	   break;
	 }
       
    }

  std::cout << "Report By Sean Hammond and Michael Sorger" << std::endl;
  std::cout << std::endl;

  typedef std::map< std::string, warehouse >::iterator outer_iterator;
  typedef std::map< std::string, food_item >::iterator food_iterator;
  typedef std::map<std::string,int>::iterator count_iterator;
  typedef std::set<std::string>::iterator set_iterator;

  std::set<std::string>unstocked;

  std::cout << "Unstocked Products:" << std::endl;

  for(food_iterator foodit = foodList.begin() ; foodit != foodList.end() ; ++foodit )
    {
      unstocked.insert(foodit->second.getUPC());
      for(outer_iterator outer = warehouseMap.begin() ; outer != warehouseMap.end() ; ++outer )
	{
	  if(outer->second.contains(foodit->second))
	    {
	      unstocked.erase(foodit->second.getUPC());
	      continue;
	    }
	}
    }
  for(set_iterator setit = unstocked.begin(); setit != unstocked.end(); ++setit)
    {
      food_iterator foodit = foodList.find(*setit);
      std::cout << foodit->second.getUPC() << " " << foodit->second.getName() << std::endl;
    }

  std::cout << std::endl;

  std::cout << "Well stocked products" << std::endl;
  std::set<std::string>wellstocked;
  bool once = false;
  for(food_iterator foodit = foodList.begin();foodit != foodList.end() ; ++foodit)
    {
      for(outer_iterator outer = warehouseMap.begin() ; outer != warehouseMap.end() ; ++outer )
	{
	  if(outer->second.contains(foodit->second))
	    {
	      if(once == true)
		{
		  wellstocked.insert(foodit->second.getUPC());	
		  once = false;
		  break;
		}
	      else
		once = true;
	    }
	}
    }

  for(set_iterator setit = wellstocked.begin(); setit != wellstocked.end(); ++setit)
    {
      food_iterator foodit = foodList.find(*setit);
      std::cout << foodit->second.getUPC() << " " << foodit->second.getName() << std::endl;
    }
 
 
}

