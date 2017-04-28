#include "warehouse.h"
#include <iostream>

/*
 * Assignment 4 Warehouse class 2/17/2017
 * Written by Sean Hammond and Michael Sorger
 */
warehouse::warehouse(){}
 warehouse::warehouse(std::string warehouseName)
  {
    this->name = warehouseName;
  }

  std::string warehouse::getName() const
  {
    return name;
  }

  // adds a food_item to the list
  void warehouse::receive(food_item stock)
  {
    //records item and quantity as recieved in correct warehouses inv
    std::map<std::string, std::vector<food_item> >::iterator it;
    it = inventory.find(stock.getUPC());
    if(it == inventory.end())
      {
	std::vector<food_item> foodL;
	foodL.push_back(stock);
	// Adds the upc food item pair to the map in the warehouse
	inventory.insert(std::pair<std::string,std::vector<food_item> >(stock.getUPC(),foodL));

      }
    else
      {
	it->second.push_back(stock);
      }
    
  }

// Helper method when the request is called in the transaction.cpp 
  void warehouse::request(food_item sell)
  {
    //Check if warehouse contains that item
    std::map<std::string, std::vector<food_item> >::iterator it;
    it = inventory.find(sell.getUPC());
      
    if(it != inventory.end())
      {
	// total amount requested
	int amount = sell.getTotal();
	// For printing purposed
	//std::cout << "Requested: "<< amount << " for " << sell.getName() << " in " << name << std::endl;

        std::vector<food_item> &itV = it->second;
	while(amount > 0) // if amount is less than 0 no need to do anything
	  {
	    if(itV.size() < 1)
	      {
		return;
	      }
	    int itVamount = itV[0].getTotal();
	    //amount = amount - itVamount;
	    if(itVamount < amount) // if the amount in the food item is less than amount requested, subtract amount and move on to next one
	      {
		amount = amount - itVamount;
		itV.erase(itV.begin());
	      }
	    else // if the amount in the food item is more than requested, subtract from the food item and stop checking
	      {
		itVamount = itVamount - amount;
		amount = 0;
		itV[0].changeQuantity(itVamount);
		return;
	      }
	  }
      }
  }

  // Update the shelf life of the product and removes if == 0
  void warehouse::update(int date)
  {
    std::map<std::string, std::vector<food_item> >::iterator it = inventory.begin();
    while(it != inventory.end())
      {
	std::vector<food_item> &itV = it->second;
	// decrements the shelf life of a food item
	for(int i = 0; i < itV.size(); i++)
	  {
	    itV[i].isBad();
	  }
	// if the shelf life of an item is 0 it must be removed
	for(int j = 0; j < itV.size(); j++)
	  {
	    if(itV[j].getLife() == 0)
	      {
		// For printing purposes
		//std::cout << "Erased " << itV[j].getName() << " from " << name << " due to expiration: " << date << " "  <<  itV[j].getLife() << std::endl;

		itV.erase(itV.begin()+j);
		// because we removed an item from the list we place j back on position so every item of the list is looked at.
		j = j - 1;
	      }
	  }
	it++;
      }
  }

// used to check if a food item is contain in the warehouse
  bool warehouse::contains(food_item product)
  {
    std::map<std::string, std::vector<food_item> >::iterator it = inventory.find(product.getUPC());
    // not found, not in the warehouse
     if(it == inventory.end())
       {
	 return false;
       }
     // item was in the warehouse at one point but doesnt contain any
     else if(it->second.size() < 1)
       {
	 return false;
       }
     return true;
  }

