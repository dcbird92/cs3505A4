#ifndef FOOD_ITEM2
#define FOOD_ITEM2

#include "food_item.h"

  food_item(std::string numberID std::string nameID, int life, int amount)
  {
    this->UPC = numberID;
    this->name = nameID;
    this->shelf_life = life;
    this->quantity = amount;
  }

  std::string getUPC const()
  {
    return UPC;
  }

  std::string getName()const
  {
    return name;
  }

  int getLife()const
  {
    return shelf_life;
  }

  int getTotal()const
  {
    return quantity;
  }
#endif
