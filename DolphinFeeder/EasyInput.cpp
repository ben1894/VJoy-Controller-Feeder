#include "easyinput.h"
#include <iostream>
#include <string>

bool cinInputValidation()
{
	if (std::cin.fail())
	{
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		return false;
	}
	return true;
}

bool cinString(std::string& str, bool errorChecking)
{
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	std::getline(std::cin, str);
	if (errorChecking == true)
	{
		if (str == "c")
		{
			return false;
		}
	}
	return true;
}

bool cinNumber(int& var, int numberCheck)
{
	std::cin >> var;
	if (!cinInputValidation())
	{
		system("CLS");
		std::cout << "Please Enter a Valid Input\n";
		system("pause");
		return false;
	}
	if (numberCheck < -9)
	{
		return true;
	}
	else
	{
		if (var > numberCheck)
		{
			system("CLS");
			std::cout << "Please Enter a Valid Selection\n";
			system("pause");
			return false;
		}
	}
	return true;
}
