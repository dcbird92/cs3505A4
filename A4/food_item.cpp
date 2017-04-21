#include "food_item.h"

/*
 * Assignment 4 Food_Item 2/17/2017
 * Written by Sean Hammond and Michael Sorger
 * Food item class to represent the food that will be held in the warehouses
 * containing get methods to return all its private variables
 */
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

// Returns the UPC of the food item
std::string food_item::getUPC()const
  {
    return UPC;
  }
// Returns the name of the food item
std::string food_item::getName()const
  {
    return name;
  }
// Returns the shelf life of the food item
int food_item::getLife()const
  {
    return shelf_life;
  }
// Returns the total amount in the food item
int food_item::getTotal()const
  {
    return quantity;
  }
// Changed the amount in the food item
void food_item::changeQuantity(int foodNum)
{
  this->quantity = foodNum;
}
// Changed the date of the food item
int food_item::setDate(int newDate)
{
  this->date = newDate; 
}
// A method called the decrement the shelf life of the item
void food_item::isBad()
{
  shelf_life = shelf_life - 1;
}
// Returns the date of the food item
int food_item::getDate()
{
  return this->date;
}
