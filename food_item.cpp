#include "food_item.h"


food_item::food_item(){}
food_item::food_item(std::string numberID, std::string nameID, int life, int amount,int day)
  {
    this->UPC = numberID;
    this->name = nameID;
    this->shelf_life = life;
    this->quantity = amount;
    this->date = day;
  }
food_item::food_item(const food_item & copy)
{
  this->UPC = copy.getUPC();
  this->name = copy.getName();
  this->shelf_life = copy.getLife();
  this->quantity = copy.getTotal();
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

void food_item::changeQuantity(int foodNum)
{
  this->quantity = foodNum;
}

int food_item::setDate(int newDate)
{
  this->date = newDate; 
}

void food_item::isBad()
{
  shelf_life = shelf_life - 1;
}
int food_item::getDate()
{
  return this->date;
}
