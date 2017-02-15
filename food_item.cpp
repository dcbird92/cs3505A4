#ifndef FOOD_ITEM2
#define FOOD_ITEM2

#include "food_item.h"

food_item::food_item(std::string numberID, std::string nameID, int life, int amount)
  {
    this->UPC = numberID;
    this->name = nameID;
    this->shelf_life = life;
    this->quantity = amount;
  }

std::string food_item::getUPC()const
  {
    return UPC;
  }

std::string food_item::getName()const
  {
    return name;
  }

int food_item::getLife()const
  {
    return shelf_life;
  }

int food_item::getTotal()const
  {
    return quantity;
  }
#endif
