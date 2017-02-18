#include "warehouse.h"
#include <iostream>

//adding a comment
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
	std::cout << "added " << stock.getTotal()<< " "  << stock.getName() << " for the first time to " << name << std::endl;
	std::vector<food_item> foodL;
	foodL.push_back(stock);
	inventory.insert(std::pair<std::string,std::vector<food_item> >(stock.getUPC(),foodL));
      }
    else
      {
	std::cout << "added " << stock.getTotal() << " " << stock.getName() << " again to  " << name << std::endl;
	it->second.push_back(stock);
      }
    // std::cout << "added " << stock.getName() << std::endl;
    
    
    //To keep track of day of which item will expire:
    //shelflife + currentDate

  }
  
  void warehouse::request(food_item sell)
  {
    //Check if warehouse contains that item
    std::map<std::string, std::vector<food_item> >::iterator it;
    it = inventory.find(sell.getUPC());
      
    if(it != inventory.end())
      {
	int amount = sell.getTotal();

	std::cout << "Requested: "<< amount << " for " << sell.getName() << " in " << name << std::endl;

        std::vector<food_item> &itV = it->second;
	while(amount > 0)
	  {
	    if(itV.size() < 1)
	      {
		return;
	      }
	    int itVamount = itV[0].getTotal();
	    //amount = amount - itVamount;
	    if(itVamount < amount)
	      {
		amount = amount - itVamount;
		itV.erase(itV.begin());
	      }
	    else
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
	for(int i = 0; i < itV.size(); i++)
	  {
	    itV[i].isBad();
	  }
	for(int j = 0; j < itV.size(); j++)
	  {
	    if(itV[j].getLife() == 0)
	      {
		std::cout << "Erased " << itV[j].getName() << " from " << name << " due to expiration: " << date << " "  <<  itV[j].getLife() << std::endl;
		itV.erase(itV.begin()+j);
		j = j - 1;
	      }
	  }
	it++;
      }
  }

  bool warehouse::contains(food_item product)
  {
    std::map<std::string, std::vector<food_item> >::iterator it = inventory.find(product.getUPC());
     if(it == inventory.end())
       {
	 return false;
       }
     else if(it->second.size() < 1)
       {
	 return false;
       }
     return true;
  }

