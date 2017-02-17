#include "transaction.h"

int main(int argc, char* argv[])
{
  std::string UPCTest = "0000000001";
  std::string nameTest = "chocolate salty balls";
  int lifeTime = 5;
  int amount = 10;

  food_item testing(UPCTest, nameTest, lifeTime, amount);
  transaction trans;
  trans.getTransactions(argv[1]);
}
