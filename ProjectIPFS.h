#pragma once

#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <openssl/evp.h>
#include <fstream>
#include <queue>
namespace fs = std::filesystem;

class UFO;
class RFTNode;
class RFT;
class MachineNode;
class DistributedHashTable;
class IPFS;


// Started this class to deal with hex numbers at first, but now it handles many misc. tasks related to strings
// setSize() function must be called before other functions. 
class UFO {
private:
	//takes a hexadecimal number in string, multiplies it by 2 and returns the result.
	static std::string hexMultiplyTwo(std::string hexNum) {
		if (hexNum == "")
			return "1";
		std::string result = "";
		int carry = 0;
		for (int i = (hexNum.length()) - 1; i >= 0; i--) {
			int digit = hexCharToInt(hexNum[i]);
			int product = carry + (digit * 2);
			carry = product / 16;
			product %= 16;
			char productChar = intToHexChar(product);
			result = productChar + result;
		}
		if (carry != 0) {
			result = intToHexChar(carry) + result;
		}
		return result;
	}
	//takes a char value in hex and returns it in decimal int
	static int hexCharToInt(char num) {
		if (num >= '0' && num <= '9') // if its from 0 to 9
			return num - 48;
		switch (num) {	//if num is from a-f
		case 'a':
			return 10;
		case 'b':
			return 11;
		case 'c':
			return 12;
		case 'd':
			return 13;
		case 'e':
			return 14;
		default:
			return 15;
		}
	}
	//takes decimal int and returns its hex value in char type
	static char intToHexChar(int num) {
		if (num >= 0 && num <= 9)
			return num + 48;
		switch (num) {
		case 10:
			return 'a';
		case 11:
			return 'b';
		case 12:
			return 'c';
		case 13:
			return 'd';
		case 14:
			return 'e';
		default:
			return 'f';
		}
	}
public:
	//size of digits that it returns 
	static int digitSize;
	static int spaceBits;
	//takes identifier space bits as input, and sets the size for future calculations with UFO class
	static void setSize(int space) {
		std::string num = hexPower_i(space);
		num = subtract(num, "1");
		digitSize = num.length();
		spaceBits = space;
	}

	//takes two strings containing hexadecimal number and returns their sum stored in string
	static std::string addHex(std::string firstHex, std::string secondHex) {
		std::string sum = ""; //stores sum in hex
		int carry = 0;

		for (int i = (firstHex.length()) - 1, j = (secondHex.length()) - 1; (i >= 0 || j >= 0 || carry != 0); i--, j--) {
			int dig_one;
			int dig_two;
			if (i >= 0)
				dig_one = hexCharToInt(firstHex[i]);
			else
				dig_one = 0;
			if (j >= 0)
				dig_two = hexCharToInt(secondHex[j]);
			else
				dig_two = 0;

			int currentDigit = dig_one + dig_two + carry;
			carry = currentDigit / 16;
			currentDigit = currentDigit % 16;
			char currentDigitChar = intToHexChar(currentDigit);
			sum = currentDigitChar + sum;
		}
		if (digitSize == -1)
			return sum;
		while (sum.length() < digitSize)
			sum = '0' + sum;
		return sum;
	}

	//takes integer i returns a string which stores hex value of 2^i
	static std::string hexPower_i(int i) {
		if (i > 160 || i < 0)
			return "-1";

		std::string result = "";

		//for (int j = 0; j <= i; j++) {
		//	result = hexMultiplyTwo(result);
		//}	 //this is the proper method.

		result = "1";
		for (int j = 1; j <= i; j++) {	//hehe, found the pattern. HEH HEH!!!
			if (result[0] == '1') {
				result[0] = '2';
			}
			else if (result[0] == '2') {
				result[0] = '4';
			}
			else if (result[0] == '4') {
				result[0] = '8';
			}
			else if (result[0] == '8') {
				result[0] = '0';
				result = '1' + result;
			}
		}
		if (digitSize == -1)
			return result;
		while (result.length() < digitSize)
			result = '0' + result;
		return result;
	}

	//compares two hex numbers.
	static bool isEqual(std::string hexOne, std::string hexTwo) {
		for (int i = 0, j = 0; i < hexOne.length() || j < hexTwo.length(); i++, j++) {
			if (i == hexOne.length() || j == hexTwo.length())
				return false;
			if (hexOne[i] != hexTwo[i])
				return false;
		}
		return true;
	}

	//subtracts second parameter from the first parameter, both of which are hexadecimal numbers in string.
	//second parameter cannot be greater than first parameter.
	static std::string subtract(std::string hexOne, std::string hexTwo) {
		if (hexTwo.length() > hexOne.length())
			return "-1";
		if (hexTwo > hexOne)
			return "-1";

		while (hexOne.length() > hexTwo.length())
			hexTwo = "0" + hexTwo;

		std::string sub = "";
		int carry = 0;
		for (int i = (hexOne.length()) - 1, j = (hexTwo.length() - 1); (j >= 0 || i >= 0); i--, j--) {
			int dig_one;
			int dig_two;
			if (i >= 0)
				dig_one = hexCharToInt(hexOne[i]);
			else
				dig_one = 0;
			if (j >= 0)
				dig_two = hexCharToInt(hexTwo[j]);
			else
				dig_two = 0;

			int currentDigit;
			if ((dig_one - carry - dig_two) < 0) {
				currentDigit = dig_one + 16 - carry - dig_two;
				carry = 1;
			}
			else {
				currentDigit = dig_one - carry - dig_two;
				carry = 0;
			}
			char currentDigitChar = intToHexChar(currentDigit);
			sub = currentDigitChar + sub;
		}
		if (digitSize == -1) {
			if (sub[0] == '0') {
				std::string newSub = "";
				for (int i = 1; i < sub.length(); i++)
					newSub += sub[i];
				return newSub;
			}
			else
				return sub;
		}

		if (sub.length() > digitSize) {
			std::string newSub = "";
			for (int i = digitSize; i > 0; i--)
				newSub = sub[i] + newSub;
			return newSub;
		}
		return sub;
	}

	//can only be called after UFO::setSize() function has been called before
	//takes hash generated from sha1 and truncates it according to digitSize
	static std::string truncateHash(std::string hexNum) {
		while (hexNum.length() < digitSize) {
			hexNum = "0" + hexNum;
		}
		std::string res = "";
		for (int i = hexNum.length() - digitSize; i < hexNum.length(); i++) {
			res += hexNum[i];
		}
		std::string maxSize = UFO::subtract(UFO::hexPower_i(spaceBits), "1");
		while (res > maxSize)
			res = subtract(res, maxSize);
		return res;
	}

	//takes a path stored in string. wherever it finds '\' , it adds another one : D
	static std::string addSlashFilePath(std::string path) {
		std::string result = "";
		for (int i = 0; i < path.size(); i++) {
			if (path[i] == '\\') {
				result += path[i];
				result += '\\';
			}
			else
				result = result + path[i];
		}
		return result;
	}

	//takes a path stored in string. constraint: path will have \\, and this will remove 1 of them.
	//if there is only one '\', the next character will be removed so make sure there are two '\'
	static std::string removeSlashFilePath(std::string path) {
		std::string result = "";
		for (int i = 0; i < path.size(); i++) {
			if (path[i] == '\\') {
				result += path[i];
				i++;
			}
			else
				result = result + path[i];
		}
		return result;
	}

	//takes path in parameters, and extracts the last part of the path which is the file name with its extension
	static std::string extractNameFromPath(std::string path) {
		std::string result = "";
		int i = path.size() - 1;
		for (; i >= 0; i--) {
			if (path[i] == '\\') {
				i++;
				break;
			}
		}
		if (i == -1)
			i = 0;
		for (int j = i; j < path.size(); j++) {
			result += path[j];
		}
		return result;
	}

	//takes file path in parameter, and remove its extension. only catering .txt :(
	static std::string removeNameExtension(std::string path) {
		std::string result = "";
		for (int i = 0; i < (path.size() - 4); i++) {
			result += path[i];
		}
		return result;
	}

	//takes filePath and checks if a file exists at that path with the same name
	//if file of that name already exists, it modifies the name appropriately, and returns the path.
	static std::string fixFilePath(std::string path) {
		if (fs::exists(path)) {
			// File already exists
			std::string suffix = "0000000000000000000000000000000000000001";
			suffix = UFO::truncateHash(suffix);
			std::string newFilename;
			std::string name = UFO::removeNameExtension(path);

			do {
				std::ostringstream oss;
				std::string newName = name;
				oss << newName << " (" << suffix << ")" << ".txt"; // Appending a number as a suffix
				newFilename = oss.str();
				suffix = UFO::addHex(suffix, "1");
			} while (fs::exists(newFilename));

			path = newFilename;
		}
		return path;
	}


}; int UFO::digitSize = -1; int UFO::spaceBits = -1;


class RFTNode
{
private:

	MachineNode* Addr;
	RFTNode* next;
	RFTNode* prev;
	int entryNumber;


public:

	friend class RFT;
	friend class MachineNode;

	//Constructor
	RFTNode(MachineNode* Addr = NULL, int entryNumber = -1, RFTNode* next = NULL, RFTNode* prev = NULL) {

		this->Addr = Addr;
		this->next = next;
		this->prev = prev;
		this->entryNumber = entryNumber;

	}

	MachineNode* getAddress()
	{
		return Addr;
	}

	RFTNode* getNext()
	{
		return next;
	}

	RFTNode* getPrev()
	{
		return prev;
	}

	int getEntryNumber()
	{
		return entryNumber;
	}

	void setAddr(MachineNode* add)
	{
		Addr = add;
	}

	void setNext(RFTNode* n)
	{
		next = n;
	}

	void setPrev(RFTNode* p)
	{
		prev = p;
	}

	void setEntry(int en)
	{
		entryNumber = en;
	}

	~RFTNode() {
		//Nothing to delete from Inside RFTNode
	}

};


//Doubly Linked List
class RFT
{
private:

	RFTNode* FTHead;
	int size;

public:

	RFTNode* getFTHead()
	{
		return FTHead;
	}

	//Constructor
	RFT(RFTNode* head, int space)
	{
		size = space;
		FTHead = head;
		//if (head) 
		//	//Here You will Create all the rows/entries (O(log(N)))
		//	std::string identifierSpace = UFO::hexPower_i(space);

		//	for (int i = 1; i <= size; i++)
		//	{
		//		std::string formula = UFO::addHex(p, UFO::hexPower_i(i - 1));

		//		if (p > identifierSpace) {
		//			p = UFO::subtract(p, identifierSpace);
		//		}

		//		insertRFTNode(findMachineSuccessor(head, p), i);
		//	}
		//}

	}

	//Create a delete Function that Only Changes the Value not removes the Memory

	//Deletes from Memory! Currently Don't Use This
	bool deleteRFTNode(MachineNode* deleteMachine) {

		/*3 Cases:
			1- Head (Move to Next Node and Set Prev to NULL)
			2- Middle (X Y Z Delete Y; X->next = Z; Z->prev = X)
			3- Last (Make 2nd Last Node Next NULL)
		*/

		if (FTHead == NULL) {
			//Empty Cannot Delete
			return false;
		}
		else {
			if (FTHead->next == NULL) {
				if (FTHead->Addr == deleteMachine) {
					delete FTHead;
					FTHead = NULL;
				}
				else {
					// Machine to delete Does not Exist
					return 0;
				}
			}
			else {
				// More than 1 Element in the List
				if (FTHead->Addr == deleteMachine) {
					//Delete From Beginning

					RFTNode* temp = FTHead;

					FTHead = FTHead->next;
					FTHead->prev = NULL;

					delete temp;

				}
				else {
					RFTNode* iterator = FTHead;
					while (iterator->next != NULL && iterator->next->Addr != deleteMachine) {
						iterator = iterator->next;
					}

					if (iterator->next == NULL) {
						if (iterator->Addr == deleteMachine) {
							// Last Node To Be Deleted

							RFTNode* temp = iterator->next;
							iterator->next = NULL;

							delete temp;

						}
						else {
							return false;
						}
					}
					else {
						if (iterator->next->Addr == deleteMachine) {
							//Deleting from Middle

							RFTNode* temp = iterator->next;

							iterator->next = temp->next;
							if (iterator->next != NULL) {
								iterator->next->prev = iterator;
							}
							delete temp;

						}
						else {
							return false;
						}
					}

				}
			}
		}

	}

	void insertRFTNode(MachineNode* newMachine, int entryNumber) {
		//Goes to the End of the list And Adds the Node there

		RFTNode* iterator = FTHead;

		if (iterator == NULL) {
			//Empty and First Entry into RT
			FTHead = new RFTNode(newMachine, entryNumber);

		}
		else {

			while (iterator->next != NULL) {
				iterator = iterator->next;
			}

			//Previous of the newNode is the Last Node in the list
			RFTNode* newNode = new RFTNode(newMachine, entryNumber, NULL, iterator);
			iterator->next = newNode;
		}

	}


	//MachineNode* findMachineSuccessor(MachineNode* head, std::string p) {
	//	MachineNode* tempIterator = head;
	//	while (tempIterator->getNext()->getMachineID() < p) {
	//		if (tempIterator->getNext() == head) {
	//			return head;
	//		}

	//		tempIterator = tempIterator->getNext();
	//	}
	//	return tempIterator;
	//}

	//Prints all the Members Starting from Head
	void traverse() {
		if (FTHead == NULL) {
			std::cout << "There are no members in the Routing Table\n";
		}
		else {
			//Change EntryNumber to MachineName Later
			RFTNode* current = FTHead;
			while (current != NULL) {
				std::cout << current->entryNumber << std::endl;
				current = current->next;
			}
		}
	}


	void printRFT()
	{
		RFTNode* tempIterator = FTHead;

		while (tempIterator->next != nullptr)
		{
			std::cout << tempIterator->Addr << "\t";
			tempIterator = tempIterator->next;
		}
		std::cout << tempIterator->Addr << "\t";
	}


	~RFT() {
		RFTNode* temp = FTHead;
		while (FTHead != NULL) {
			FTHead = FTHead->next;
			delete temp;
			temp = FTHead;
		}

	}

};


class BTreeNode {
private:
	int count;
	std::string* key;
	std::string* paths;
	BTreeNode** childs;
	int MAX;
	int MIN;

public:
	friend class BTree;

	BTreeNode(int MAX = 0, int MIN = 0, std::string newkey = "", std::string newpath = "") {
		count = -1;
		key = new std::string[MAX + 1];
		paths = new std::string[MAX + 1];
		childs = new BTreeNode * [MAX + 1];
		for (int i = 0; i < MAX + 1; i++) {
			key[i] = "";
			paths[i] = "";
			childs[i] = NULL;
		}
		this->MAX = MAX;
		this->MIN = MIN;

		if (newkey != "") {
			count = 1;
			key[1] = newkey;
			paths[1] = newpath;
		}
	}

	std::string getSmallestKey() {
		return key[1];
	}

	std::string getGreatestKey() {
		return key[count];
	}

	std::string getSmallestPath() {
		return paths[1];
	}
	std::string getGreatestPath() {
		return paths[count];
	}

	BTreeNode* insert(std::string val, std::string newpath, BTreeNode* root) {

		std::string i, ii;
		BTreeNode* c, * n;
		int flag;

		flag = setval(val, newpath, root, &i, &ii, &c);

		if (flag) {
			n = new BTreeNode(MAX, MIN);
			n->count = 1;
			n->key[1] = i;
			n->paths[1] = ii;
			n->childs[0] = root;
			n->childs[1] = c;
			return n;
		}
		return root;

	}

	int setval(std::string val, std::string newpath, BTreeNode* n, std::string* p, std::string* pp, BTreeNode** c) {

		int k;
		if (n == NULL) {
			*p = val;
			*pp = newpath;
			*c = NULL;
			return 1;
		}
		else {
			//We will also Add Duplicates
			if (searchnode(val, n, &k)) {
				//Already Exists
				n->paths[k] = n->paths[k] + '\n' + newpath;
				return 0;
			}

			if (setval(val, newpath, n->childs[k], p, pp, c)) {
				if (n->count < MAX) {
					fillnode(*p, *pp, *c, n, k);
					return 0;
				}
				else {
					split(*p, *pp, *c, n, k, p, pp, c);
					return 1;
				}
			}
		}
		return 0;
	}

	void fillnode(std::string val, std::string newpath, BTreeNode* c, BTreeNode* n, int k) {
		int i;
		for (i = n->count; i > k; i--) {
			n->key[i + 1] = n->key[i];
			n->paths[i + 1] = n->paths[i];
			n->childs[i + 1] = n->childs[i];
		}
		n->key[k + 1] = val;
		n->paths[k + 1] = newpath;
		n->childs[k + 1] = c;
		n->count++;
	}

	void split(std::string val, std::string newpath, BTreeNode* c, BTreeNode* n, int k, std::string* y, std::string* yy, BTreeNode** newNode) {

		int i, mid;
		if (k <= MIN) {
			mid = MIN;
		}
		else {
			mid = MIN + 1;
		}

		*newNode = new BTreeNode(MAX, MIN);

		for (i = mid + 1; i <= MAX; i++) {
			(*newNode)->key[i - mid] = n->key[i];
			(*newNode)->paths[i - mid] = n->paths[i];
			(*newNode)->childs[i - mid] = n->childs[i];
		}
		(*newNode)->count = MAX - mid;
		n->count = mid;

		if (k <= MIN) {
			fillnode(val, newpath, c, n, k);
		}
		else {
			fillnode(val, newpath, c, *newNode, k - mid);
		}
		*y = n->key[n->count];
		*yy = n->paths[n->count];
		(*newNode)->childs[0] = n->childs[n->count];
		n->count--;
	}
	int searchnode(std::string val, BTreeNode* n, int* pos) {
		if (val < n->key[1]) {
			*pos = 0;
			return 0;
		}
		else {
			*pos = n->count;
			while ((val < n->key[*pos]) && *pos > 1) {
				(*pos)--;
			}
			if (val == n->key[*pos]) {
				return 1;
			}
			else {
				return 0;
			}
		}
	}

	BTreeNode* search(std::string val, BTreeNode* root, int* pos) {

		if (root == NULL) {
			return NULL;
		}
		else {
			if (searchnode(val, root, pos)) {
				return root;
			}
			else {
				return search(val, root->childs[*pos], pos);
			}
		}

	}

	int delhelp(std::string val, BTreeNode* root) {

		int i;
		int flag;
		if (root == NULL) {
			return 0;
		}
		else {
			flag = searchnode(val, root, &i);
			if (flag) {
				if (root->childs[i - 1]) {
					copysucc(root, i);
					flag = delhelp(root->key[i], root->childs[i]);
					if (!flag) {
						//key was Not Found
					}
				}
				else {
					clear(root, i);
				}
			}
			else {
				flag = delhelp(val, root->childs[i]);
			}
		}

		if (root->childs[i] != NULL) {
			if (root->childs[i]->count < MIN) {
				restore(root, i);
			}
		}
		return flag;

	}

	void copysucc(BTreeNode* node, int i) {
		BTreeNode* temp;
		temp = node->childs[i];

		//I Changed the Code Here
		while (temp->childs[0]) {
			temp = temp->childs[0];
		}
		node->key[i] = temp->key[1];
		node->paths[i] = temp->paths[1];
	}

	void clear(BTreeNode* node, int k) {
		int i;
		for (i = k + 1; i <= node->count; i++) {
			node->key[i - 1] = node->key[i];
			node->paths[i - 1] = node->paths[i];
			node->childs[i - 1] = node->childs[i];
		}

		node->count--;
	}

	void restore(BTreeNode* node, int i) {
		if (i == 0) {
			//I Changed This Here
			if (node->childs[i + 1]->count > MIN) {
				leftshift(node, 1);
			}
			else {
				merge(node, 1);
			}
		}
		else {
			if (i == node->count) {
				if (node->childs[i - 1]->count > MIN) {
					rightshift(node, i);
				}
				else {
					merge(node, i);
				}
			}
			else {
				if (node->childs[i - 1]->count > MIN) {
					rightshift(node, i);
				}
				else {
					if (node->childs[i + 1]->count > MIN) {
						leftshift(node, i + 1);
					}
					else {
						merge(node, i);
					}
				}
			}
		}
	}

	void rightshift(BTreeNode* node, int k) {
		int i;
		BTreeNode* temp;

		temp = node->childs[k];

		for (i = temp->count; i > 0; i--) {
			temp->key[i + 1] = temp->key[i];
			temp->paths[i + 1] = temp->paths[i];
			temp->childs[i + 1] = temp->childs[i];
		}

		temp->childs[1] = temp->childs[0];
		temp->count++;
		temp->key[1] = node->key[k];
		temp->paths[1] = node->paths[k];

		temp = node->childs[k - 1];
		node->key[k] = temp->key[temp->count];
		node->paths[k] = temp->paths[temp->count];
		node->childs[k]->childs[0] = temp->childs[temp->count];
		temp->count--;
	}

	void leftshift(BTreeNode* node, int k) {
		int i;
		BTreeNode* temp;

		temp = node->childs[k - 1];
		temp->count++;
		temp->key[temp->count] = node->key[k];
		temp->paths[temp->count] = node->paths[k];
		temp->childs[temp->count] = node->childs[k]->childs[0];

		temp = node->childs[k];
		node->key[k] = temp->key[1];
		node->paths[k] = temp->paths[1];
		temp->childs[0] = temp->childs[1];
		temp->count--;

		for (i = 1; i <= temp->count; i++) {
			temp->key[i] = temp->key[i + 1];
			temp->paths[i] = temp->paths[i + 1];
			temp->childs[i] = temp->childs[i + 1];
		}
	}

	void merge(BTreeNode* node, int k) {
		int i;
		BTreeNode* temp1, * temp2;

		temp1 = node->childs[k];
		temp2 = node->childs[k - 1];
		temp2->count++;
		temp2->key[temp2->count] = node->key[k];
		temp2->paths[temp2->count] = node->paths[k];
		//I Added this Condition Line Here
		if (temp2->childs[0] != NULL) {
			temp2->childs[temp2->count] = temp1->childs[0];
		}

		for (i = 1; i <= temp1->count; i++) {
			temp2->count++;
			temp2->key[temp2->count] = temp1->key[i];
			temp2->paths[temp2->count] = temp1->paths[i];
			temp2->childs[temp2->count] = temp1->childs[i];
		}
		for (i = k; i < node->count; i++) {
			node->key[i] = node->key[i + 1];
			node->paths[i] = node->paths[i + 1];
			node->childs[i] = node->childs[i + 1];
		}
		node->count--;
		delete temp1;
	}

	BTreeNode* del(std::string val, BTreeNode* root) {
		BTreeNode* temp;
		if (!delhelp(val, root)) {
			//key Not Found
			std::cout << "The File to be deleted was not found \n";
		}
		else {
			if (root->count == 0) {
				temp = root;
				root = root->childs[0];
				delete temp;
			}
		}
		return root;
	}

	void generateDot(std::ofstream& dotFile, int& nodeCounter, bool isPath = false) {
		int currentNodeIndex = nodeCounter++;

		dotFile << "node" << currentNodeIndex << "[label=\"";
		for (int i = 1; i <= count; ++i) {
			if (!isPath)
				dotFile << "<f" << i << "> |" << key[i] << "|";
			else
				dotFile << "<f" << i << "> |" << UFO::addSlashFilePath(paths[i]) << "|";
			if (i < count - 1) {
				dotFile << " ";
			}
		}
		dotFile << "<f" << count + 1 << ">\"];" << std::endl;

		// Traverse the children
		for (int i = 0; i <= count; ++i) {
			if (childs[i] != nullptr) {
				int childNodeIndex = nodeCounter;
				childs[i]->generateDot(dotFile, nodeCounter, isPath);

				dotFile << "\"node" << currentNodeIndex << "\":f" << i + 1 << " -> \"node" << childNodeIndex << "\"" << std::endl;
			}
		}
	}

};

class BTree {
private:

	BTreeNode* root;
	int	M;
public:

	BTree(int M) {
		root = NULL;
		this->M = M;
	}

	void insert(std::string key, std::string path) {
		if (root == NULL) {
			root = new BTreeNode(M - 1, ceil(((float)M / 2)) - 1, key, path);
		}
		else {
			root = root->insert(key, path, root);
		}
	}

	void deleteNode(std::string key) {
		if (root == NULL) {
			//Cannot Delete
		}
		else {
			root = root->del(key, root);
		}
	}
	void printHelper(BTreeNode* iterator) {

		std::cout << " | ";
		for (int i = 1; i <= iterator->count; i++) {

			if (i == iterator->count) {
				std::cout << iterator->key[i];
			}
			else {
				std::cout << iterator->key[i] << "-";
			}


		}
		std::cout << " | ";

	}

	void printBTree() {
		BTreeNode* iterator = root;

		std::queue<BTreeNode*> myQueue1;
		std::queue<BTreeNode*> myQueue2;

		if (root != NULL) {
			myQueue1.push(iterator);
		}

		while (!myQueue1.empty() || !myQueue2.empty()) {

			while (!myQueue1.empty()) {
				iterator = myQueue1.front();
				myQueue1.pop();

				printHelper(iterator);
				//std::cout<<'\t';

				if (iterator->childs[0] != NULL) {
					//Childs Will be One Greater than the Count
					for (int i = 0; i < iterator->count + 1; i++) {
						myQueue2.push(iterator->childs[i]);
					}
				}
			}

			std::cout << "\n";

			while (!myQueue2.empty()) {
				iterator = myQueue2.front();
				myQueue2.pop();

				printHelper(iterator);
				//std::cout << '\t';

				if (iterator->childs[0] != NULL) {
					//Childs Will be One Greater than the Count
					for (int i = 0; i < iterator->count + 1; i++) {
						myQueue1.push(iterator->childs[i]);
					}
				}
			}

			std::cout << "\n";
		}

	}

	BTreeNode* smallestNode() {
		if (root == NULL) {
			return NULL;
		}
		else {
			BTreeNode* iterator = root;
			while (iterator->childs[0] != NULL) {
				iterator = iterator->childs[0];
			}
			return iterator;
		}
	}

	BTreeNode* greatestNode() {
		if (root == NULL) {
			return NULL;
		}
		else {
			BTreeNode* iterator = root;
			while (iterator->childs[iterator->count] != NULL) {
				iterator = iterator->childs[iterator->count];
			}
			return iterator;
		}
	}

	std::string search(std::string fileKey) {

		BTreeNode* TheNode;
		int pos;

		TheNode = root->search(fileKey, root, &pos);
		if (TheNode == NULL) {
			std::cout << "The File Does Not Exist\n";
			return "";
		}
		else {
			std::cout << "The Files for the Key " << fileKey << " are: \n";
			std::cout << TheNode->paths[pos] << '\n';
			return TheNode->paths[pos];
		}
	}

	//Creates PNG of BTree of a Particular Machine. Call This Function from BT of any Machine
	void generateBTreeDotFile() {
		std::ofstream dotFile("btree.dot");
		dotFile << "digraph BTree { node [shape = record];" << std::endl;

		int nodeCounter = 0;

		if (root != nullptr) {
			std::cout << "Enter 0 for Keys And 1 for Paths: ";
			bool isPath = false;
			std::cin >> isPath;
			root->generateDot(dotFile, nodeCounter, isPath);
		}

		dotFile << "}" << std::endl;

		dotFile.close();

		std::string command = "dot -Tpng -O btree.dot";

		// Execute the command
		int result = std::system(command.c_str());

		if (result == 0) {
			std::cout << "Graph generated successfully." << std::endl;
			std::system("start btree.dot.png");
		}
		else {
			std::cerr << "Error generating graph." << std::endl;
		}
	}

};


class MachineNode
{
private:
	std::string machineID;
	std::string machineName;
	RFT* routingTable;

	int identifierSpaceInt;
	BTree* BT;

	// The object for RFT is a pointer

	MachineNode* next;



public:
	MachineNode(std::string id = "", std::string name = "", MachineNode* newNext = nullptr, int space = 0, int order = 5)
	{
		machineID = id;
		machineName = name;
		next = newNext;

		identifierSpaceInt = space;
		// Pass the MHead to the RFT object

		// routingTable = new RFT(head, id, space);
		routingTable = nullptr;

		BT = new BTree(order);

	}

	BTree* getBTreeHead()
	{
		return BT;
	}

	RFT* getRoutingTable()
	{
		return routingTable;
	}

	void printRFT()
	{
		RFTNode* tempIterator = routingTable->getFTHead();

		while (tempIterator->getNext() != nullptr)
		{
			std::cout << tempIterator->getAddress()->getMachineName() << "\t";
			tempIterator = tempIterator->next;
		}
		std::cout << tempIterator->getAddress()->getMachineName() << "\t";
	}

	void setNext(MachineNode* n)
	{
		next = n;

	}

	MachineNode* getNext()
	{
		return next;
	}

	std::string getMachineID() const
	{
		return machineID;
	}

	void setMachineID(std::string id)
	{
		machineID = id;
	}

	std::string getMachineName()
	{
		return machineName;
	}

	void setMachineName(std::string name)
	{
		machineName = name;
	}

	void reconstructRFT(MachineNode* head)
	{
		RFTNode* FTHead = nullptr;

		//Here You will Create all the rows/entries (O(log(N)))
		std::string identifierSpace = UFO::hexPower_i(identifierSpaceInt);
		std::string p = machineID;

		for (int i = 1; i <= identifierSpaceInt; i++)
		{
			std::string formula = UFO::addHex(p, UFO::hexPower_i(i - 1));

			if (formula.length() > identifierSpace.length())
				formula = UFO::subtract(formula, identifierSpace);
			else if (formula.length() == identifierSpace.length() && formula >= identifierSpace) {
				formula = UFO::subtract(formula, identifierSpace);
				formula = UFO::truncateHash(formula);
			}
			else if (formula > UFO::subtract(identifierSpace, "1"))
				formula = UFO::subtract(formula, identifierSpace);

			addRFTNodeToMachine(FTHead, findMachineSuccessor(head, formula), i);

			p = machineID;
		}

		if (routingTable == nullptr)
		{
			routingTable = new RFT(FTHead, identifierSpaceInt);
		}
		else
		{
			delete routingTable;
			routingTable = new RFT(FTHead, identifierSpaceInt);

		}

	}

	MachineNode* findMachineSuccessor(MachineNode* head, std::string p) {
		MachineNode* tempIterator = head;

		if (tempIterator->getMachineID() >= p)
		{
			return tempIterator;
		}

		while (tempIterator->getNext()->getMachineID() < p) {
			if (tempIterator->getNext() == head) {
				return head;
			}

			tempIterator = tempIterator->getNext();
		}
		return tempIterator->getNext();
	}

	void addRFTNodeToMachine(RFTNode*& FTHead, MachineNode* newMachine, int entryNumber)
	{
		//Goes to the End of the list And Adds the Node there

		RFTNode* iterator = FTHead;

		if (iterator == NULL) {
			//Empty and First Entry into RT
			FTHead = new RFTNode(newMachine, entryNumber);

		}
		else {

			while (iterator->next != NULL) {
				iterator = iterator->getNext();
			}


			//Previous of the newNode is the Last Node in the list
			RFTNode* newNode = new RFTNode(newMachine, entryNumber, NULL, iterator);
			iterator->setNext(newNode);
		}

	}


	MachineNode* search(std::string hashValue)
	{
		MachineNode* tempIterator = this;

		while (true)
		{
			if (hashValue == tempIterator->getMachineID())
			{
				return tempIterator;
			}

			// if p < e && (e > FT[1] || p > FT[1]) // the p greater than FT[1] part adds the modulus functionality to the searching algorithm
			if (tempIterator->getMachineID() < hashValue && (hashValue <= tempIterator->getRoutingTable()->getFTHead()->getAddress()->getMachineID() || tempIterator->getMachineID() > tempIterator->getRoutingTable()->getFTHead()->getAddress()->getMachineID()))
			{
				return tempIterator->getRoutingTable()->getFTHead()->getAddress();
			}

			// modulus case for when the hash value is less than the current machine
			// FT[1] < p is modulus wala case
			if (tempIterator->getMachineID() > hashValue && (tempIterator->getRoutingTable()->getFTHead()->getAddress()->getMachineID() <= tempIterator->getMachineID() && tempIterator->getRoutingTable()->getFTHead()->getAddress()->getMachineID() >= hashValue))
			{
				return tempIterator->getRoutingTable()->getFTHead()->getAddress();
			}

			RFTNode* RFTiterator = tempIterator->getRoutingTable()->getFTHead();

			for (int i = 1; i <= identifierSpaceInt; i++)
			{
				if (RFTiterator->getAddress()->getMachineID() == hashValue)
				{
					tempIterator = RFTiterator->getAddress();
					break;
				}

				if (hashValue >= RFTiterator->getAddress()->getMachineID() && i == identifierSpaceInt)
				{
					tempIterator = RFTiterator->getAddress();
					break;
				}

				if (hashValue <= RFTiterator->getAddress()->getMachineID() && i == identifierSpaceInt)
				{
					tempIterator = RFTiterator->getAddress();
					break;
				}

				if (RFTiterator->getAddress()->getMachineID() < hashValue && RFTiterator->getNext()->getAddress()->getMachineID() < RFTiterator->getAddress()->getMachineID())
				{
					tempIterator = RFTiterator->getAddress();
					break;
				}

				if (hashValue > RFTiterator->getAddress()->getMachineID() && hashValue >= RFTiterator->getNext()->getAddress()->getMachineID())
				{
					RFTiterator = RFTiterator->getNext();
					continue;
				}


				if (RFTiterator->getAddress()->getMachineID() > hashValue && RFTiterator->getNext()->getAddress()->getMachineID() < RFTiterator->getAddress()->getMachineID())
				{
					tempIterator = RFTiterator->getAddress();
					break;
				}

				if (hashValue < RFTiterator->getAddress()->getMachineID() && hashValue <= RFTiterator->getNext()->getAddress()->getMachineID())
				{
					RFTiterator = RFTiterator->getNext();
					continue;
				}
				if (hashValue > RFTiterator->getAddress()->getMachineID() && hashValue <= RFTiterator->getNext()->getAddress()->getMachineID())
				{
					tempIterator = RFTiterator->getAddress();
					break;
				}


			}
		}
	}

	bool fileAdd(std::string filePath, std::string fileHash)
	{

		BT->insert(fileHash, filePath);
		return true;
	}

	void fileSearch(std::string fileKey) {
		BT->search(fileKey);
	}

	bool fileRemove(std::string key, bool reassigning = 0) {

		//CHANGES
		//Ask which File Amongst the Duplicates to Delete

		std::string temp = BT->search(key);
		std::string deletedString = "";

		if (temp == "") {
			return 0;
		}

		int i = 0;
		int count = 1;

		//Now We will Count the Number of Records in this Node
		while (temp[i] != '\0') {
			if (temp[i] == '\n') {
				count++;
			}
			i++;
		}

		if (count == 1) {
			BT->deleteNode(key);
			// case 1
			fs::remove(temp);
			// Check if the file still exists
			if (std::filesystem::exists(temp)) {
				std::cerr << "Error: File was not deleted successfully." << std::endl;
			}
			else {
				std::cout << "File deleted successfully." << std::endl;
			}
			return true;
		}

		if (reassigning)
		{
			// case 2
			int ind = 0;
			for (int i = 0; i < count; i++)
			{
				// Delete all duplicated files
				std::string currentFile = "";


				while (temp[ind] != '\n' && temp[ind] != '\0')
				{
					currentFile += temp[ind];
					ind++;
				}
				ind++;
				// now we have the path for the current file
				fs::remove(currentFile);
				// Check if the file still exists

				if (std::filesystem::exists(currentFile)) {
					std::cerr << "Error: File was not deleted successfully." << std::endl;
				}
				else {
					std::cout << "File deleted successfully." << std::endl;
				}

			}

			BT->deleteNode(key);
			return true;

		}

		std::cout << "Which file would you like to Delete? Enter 0 To Delete All the Files! Must Enter a Number: ";

		int choice;

		do {
			std::cin >> choice;
			std::cin.ignore();
			if (choice > count || choice < 0) {
				std::cout << "Please Enter a Valid Option: ";
			}
		} while (choice > count || choice < 0);


		if (choice == 0) {
			// case 2
			int ind = 0;
			for (int i = 0; i < count; i++)
			{
				// Delete all duplicated files
				std::string currentFile = "";


				while (temp[ind] != '\n' && temp[ind] != '\0')
				{
					currentFile += temp[ind];
					ind++;
				}
				ind++;
				// now we have the path for the current file
				fs::remove(currentFile);
				// Check if the file still exists

				if (std::filesystem::exists(currentFile)) {
					std::cerr << "Error: File was not deleted successfully." << std::endl;
				}
				else {
					std::cout << "File deleted successfully." << std::endl;
				}

			}

			BT->deleteNode(key);
		}
		else {

			BT->deleteNode(key);

			std::string newPath = "";

			//Now Everything of that Key is Deleted and We will insert Back the File With the Modified String
			//Case that only 1 File is there and User Entered 1
			int i = 0;

			if (choice == 1) {
				//Delete One Record from the Beginning
				while (temp[i] != '\n' && temp[i] != '\0') {
					deletedString = deletedString + temp[i];
					i++;
				}
				deletedString = deletedString + '\0';

				if (temp[i] != '\0') {
					i++;
					while (temp[i] != '\0') {
						newPath = newPath + temp[i];
						i++;
					}
					newPath = newPath + '\0';
				}

			}
			else if (choice == count) {
				//Deleting the Last Record from the Duplicates
				int i = 0;
				int innerCount = 0;

				while (temp[i] != '\0') {

					if (temp[i] == '\n') {
						innerCount++;
					}

					if (innerCount == count - 1) {
						break;
					}

					newPath = newPath + temp[i];
					i++;

				}
				i++;
				newPath = newPath + '\0';

				while (temp[i] != '\0') {
					deletedString = deletedString + temp[i];
					i++;
				}

				deletedString = deletedString + '\0';

			}
			else {
				//Deleting A record from the Middle

				int i = 0;
				int innerCount = 0;

				while (temp[i] != '\0') {

					if (temp[i] == '\n') {
						innerCount++;
					}

					if (innerCount == choice - 1) {
						break;
					}

					newPath = newPath + temp[i];
					i++;

				}

				i++;
				while (temp[i] != '\n') {
					deletedString = deletedString + temp[i];
					i++;
				}
				deletedString = deletedString + '\0';

				while (temp[i] != '\0') {
					newPath = newPath + temp[i];
					i++;
				}

				newPath = newPath + '\0';
			}
			BT->insert(key, newPath);

			fs::remove(deletedString);
			// Check if the file still exists
			if (std::filesystem::exists(deletedString)) {
				std::cerr << "Error: File was not deleted successfully." << std::endl;
			}
			else {
				std::cout << "File deleted successfully." << std::endl;
			}

		}

	}

	void createFileInNewDirectory(MachineNode* currentNode, std::string oldPath, std::string Key, bool reassigning = 0)
	{
		std::ifstream readFile(oldPath);
		std::string fileContent = "";

		if (!readFile)
		{
			std::cout << "\nThe file path you have provided is invalid!\n";
			//return false;
		}

		std::ostringstream ss;

		ss << readFile.rdbuf();
		fileContent = ss.str();

		readFile.close();

		std::string newFilePath = ".\\Machines\\" + currentNode->getMachineName() + "_" + currentNode->getMachineID() + "\\" + UFO::removeNameExtension(UFO::extractNameFromPath(oldPath));
		if (!reassigning)
			newFilePath = newFilePath + "_" + Key + ".txt";
		else
			newFilePath = newFilePath + ".txt";

		newFilePath = UFO::fixFilePath(newFilePath);

		std::ofstream fileAdd(newFilePath);

		if (!fileAdd.is_open())
		{
			std::cout << "\nUnable to open file " << UFO::extractNameFromPath(newFilePath) << "\n";
		}
		else
		{
			fileAdd << fileContent;
			fileAdd.close();
		}
	}

	// Put these in UFO class
	int countDuplicates(std::string filePath)
	{
		int i = 0;
		int count = 1;

		while (filePath[i] != '\0')
		{
			if (filePath[i] == '\n')
			{
				count++;
			}
			i++;
		}

		return count;
	}

	std::string extract(std::string dupFilePath, int numFile)
	{
		std::string extractedFile = "";

		int totalCount = countDuplicates(dupFilePath);
		int numDup = 0;

		std::string newPath = "";

		//Now Everything of that Key is Deleted and We will insert Back the File With the Modified String
		//Case that only 1 File is there and User Entered 1


		if (numFile == 1) {
			int i = 0;
			//Delete One Record from the Beginning
			while (dupFilePath[i] != '\n' && dupFilePath[i] != '\0') {
				extractedFile += dupFilePath[i];
				i++;
			}

			return extractedFile;

		}
		else if (numFile == totalCount) {
			//Deleting the Last Record from the Duplicates
			int i = 0;
			int innerCount = 0;

			while (dupFilePath[i] != '\0') {

				if (dupFilePath[i] == '\n') {
					innerCount++;
				}

				if (innerCount == totalCount - 1) {
					break;
				}
				i++;
			}
			i++;

			while (dupFilePath[i] != '\0') {
				extractedFile += dupFilePath[i];
				i++;
			}


			return extractedFile;
		}
		else {
			//Deleting A record from the Middle

			int i = 0;
			int innerCount = 0;

			while (dupFilePath[i] != '\0') {

				if (dupFilePath[i] == '\n') {
					innerCount++;
				}

				if (innerCount == numFile - 1) {
					break;
				}
				i++;
			}

			i++;
			while (dupFilePath[i] != '\n') {
				extractedFile += dupFilePath[i];
				i++;
			}

			return extractedFile;
		}


	}


	//is called when a new machine is added, pulls its own files from its next machine.
	void pullFiles() {
		if (next == this)	//if there is only one node, no need to assign files.
			return;

		BTreeNode* smallest = next->BT->smallestNode();
		BTreeNode* greatest = next->BT->greatestNode();

		if (!smallest || !greatest)
			return;
		// change the second smallest key to get greatest key
		while (smallest->getSmallestKey() <= machineID || greatest->getGreatestKey() > next->getMachineID()) {

			if (greatest->getGreatestKey() > next->getMachineID()) {
				std::string greatestKey = greatest->getGreatestKey();
				std::string greatestPath = greatest->getGreatestPath();
				std::string newPath;
				// Now before deleting this file, we need to access the file and read the content so that we can reassign it to the other directory

				int duplicates = countDuplicates(greatestPath);

				if (duplicates == 1)
				{
					newPath = ".\\Machines\\" + this->getMachineName() + "_" + this->getMachineID() + "\\" + UFO::extractNameFromPath(greatestPath);
					createFileInNewDirectory(this, greatestPath, greatestKey, 1);
					fileAdd(newPath, greatestKey);
				}
				else
				{
					for (int i = 1; i <= duplicates; i++)
					{
						std::string filePath = extract(greatestPath, i);
						newPath = ".\\Machines\\" + this->getMachineName() + "_" + this->getMachineID() + "\\" + UFO::extractNameFromPath(filePath);
						createFileInNewDirectory(this, filePath, greatestKey, 1);
						fileAdd(newPath, greatestKey);
					}
				}
				// BTRee me addition horahi hai idhar

				//BT->insert(greatestKey, greatestPath);

				// Btree se removal horahi hai
				next->fileRemove(greatestKey, 1);
				//next->BT->deleteNode(greatestKey);
			}
			else {
				std::string smallestKey = smallest->getSmallestKey();
				std::string smallestPath = smallest->getSmallestPath();
				std::string newPath;
				// Now before deleting this file, we need to access the file and read the content so that we can reassign it to the other directory

				int duplicates = countDuplicates(smallestPath);

				if (duplicates == 1)
				{
					newPath = ".\\Machines\\" + this->getMachineName() + "_" + this->getMachineID() + "\\" + UFO::extractNameFromPath(smallestPath);
					createFileInNewDirectory(this, smallestPath, smallestKey, 1);
					fileAdd(newPath, smallestKey);
				}
				else
				{
					for (int i = 1; i <= duplicates; i++)
					{
						std::string filePath = extract(smallestPath, i);
						newPath = ".\\Machines\\" + this->getMachineName() + "_" + this->getMachineID() + "\\" + UFO::extractNameFromPath(filePath);
						createFileInNewDirectory(this, filePath, smallestKey, 1);
						fileAdd(newPath, smallestKey);
					}
				}

				next->fileRemove(smallestKey, 1);
				//next->BT->deleteNode(smallestKey);


			}
			smallest = next->BT->smallestNode();
			greatest = next->BT->greatestNode();
			if (!smallest || !greatest)
				return;

		}
	}

	//is called when a new machine is deleted, push its own files to the next machine before its deleted
	void pushFiles() {
		if (next == this)	//if there is only one node, no need to assign files.
			return;

		BTreeNode* smallest = BT->smallestNode();


		// change the second smallest key to get greatest key
		while (smallest) {
			std::string smallestKey = smallest->getSmallestKey();
			std::string smallestPath = smallest->getSmallestPath();
			std::string newPath;

			int duplicates = countDuplicates(smallestPath);

			if (duplicates == 1)
			{
				newPath = ".\\Machines\\" + next->getMachineName() + "_" + next->getMachineID() + "\\" + UFO::extractNameFromPath(smallestPath);
				createFileInNewDirectory(next, smallestPath, smallestKey, 1);
				next->fileAdd(newPath, smallestKey);
			}
			else
			{
				for (int i = 1; i <= duplicates; i++)
				{
					std::string filePath = extract(smallestPath, i);
					newPath = ".\\Machines\\" + next->getMachineName() + "_" + next->getMachineID() + "\\" + UFO::extractNameFromPath(filePath);
					createFileInNewDirectory(next, filePath, smallestKey, 1);
					next->fileAdd(newPath, smallestKey);
				}
			}

			fileRemove(smallestKey, 1);
			//BT->deleteNode(smallestKey);


			smallest = BT->smallestNode();
			if (!smallest)
				return;
		}
	}

	void generateDotRFT() {

		int currentnodeIndex = 0;
		RFTNode* current = routingTable->getFTHead();

		std::ofstream dotFile("RFT.dot");
		dotFile << "digraph RoutingTable { node[shape=record]; rankdir=TB; edge[tailclip=false,arrowtail=dot,dir=both];" << std::endl;


		while (current->getNext() != NULL) {


			dotFile << "p" << currentnodeIndex << "[label=\"{" << "<data>" << current->getAddress()->getMachineName() << "|" << "<data>" << current->getAddress()->getMachineID() << "|" << "<next>" << "}\"];\n";

			dotFile << "p" << currentnodeIndex << ":next:c -> p" << currentnodeIndex + 1 << ":data;\n";

			current = current->getNext();
			currentnodeIndex++;
		}
		//Now Add One Here as Well

		dotFile << "p" << currentnodeIndex << "[label=\"{" << "<data>" << current->getAddress()->getMachineName() << "|" << "<data>" << current->getAddress()->getMachineID() << "|" << "<next>" << "}\"];\n";
		dotFile << "}" << std::endl;

		dotFile.close();

		std::string command = "dot -Tpng -O RFT.dot";
		// Execute the command
		int result = std::system(command.c_str());

		if (result == 0) {
			std::cout << "Graph generated successfully." << std::endl;
			std::system("start RFT.dot.png");
		}
		else {
			std::cerr << "Error generating graph." << std::endl;
		}
	}


};




class DistributedHashTable
{
private:
	MachineNode* MHead;
	bool manual;
	std::string totalMachines;
	std::string identifierSpace;
	int identifierSpaceInt;
	int orderOfBTree;

	bool InsertInLinkedList(MachineNode* node)
	{
		if (MHead == nullptr)
		{
			MHead = node;
			MHead->setNext(nullptr);
		}
		else
		{
			MachineNode* tempIterator = MHead;

			while (tempIterator->getNext() != MHead)
			{
				tempIterator = tempIterator->getNext();
			}

			tempIterator->setNext(node);
			tempIterator->getNext()->setNext(MHead);
			return true;
		}
		return false;
	}

	bool RemoveFromLinkedList(MachineNode* node)
	{
		if (MHead == nullptr)
		{
			return false;
		}
		else
		{
			MachineNode* tempIterator = MHead;

			while (tempIterator->getNext()->getMachineID() != node->getMachineID())
			{
				tempIterator = tempIterator->getNext();
			}

			MachineNode* temp = tempIterator->getNext();

			tempIterator = tempIterator->getNext()->getNext();
			return true;
		}
		return false;
	}


public:

	DistributedHashTable(int order, int space)
	{
		MHead = nullptr;
		manual = false;
		totalMachines = "0000000000000000000000000000000000000000";

		// maybe in IPFS?
		UFO::setSize(space);

		identifierSpace = UFO::hexPower_i(space);
		identifierSpaceInt = space;

		totalMachines = UFO::truncateHash(totalMachines);
		orderOfBTree = order;

	}

	bool getManual()
	{
		return manual;
	}

	void setManual() {
		manual = true;
	}
	MachineNode* getMHead()
	{
		return MHead;
	}

	std::string sha1(const std::string& input)
	{
		EVP_MD_CTX* mdctx;
		const EVP_MD* md;
		unsigned char hash[EVP_MAX_MD_SIZE];
		unsigned int hashLen;

		OpenSSL_add_all_digests();
		md = EVP_get_digestbyname("sha1");

		if (!md)
		{
			std::cerr << "Error: SHA-1 not supported." << std::endl;
			return "";
		}

		mdctx = EVP_MD_CTX_new();
		EVP_DigestInit_ex(mdctx, md, nullptr);
		EVP_DigestUpdate(mdctx, input.c_str(), input.length());
		EVP_DigestFinal_ex(mdctx, hash, &hashLen);
		EVP_MD_CTX_free(mdctx);

		std::stringstream ss;
		for (unsigned int i = 0; i < hashLen; ++i)
		{
			ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
		}

		return ss.str();
	}

	bool createDirectory(std::string machineName, std::string machineID)
	{
		std::string folderName = machineName + "_" + machineID;
		folderName = ".\\Machines\\" + folderName;
		// Check if the folder already exists
		if (!fs::exists(folderName)) {
			// Create the directory
			if (fs::create_directory(folderName)) {
				std::cout << "\nFolder created successfully!\n";
				return true;
			}
			else {
				std::cout << "\nFailed to create folder.\n";
			}
		}
		else {
			std::cout << "\nFolder already exists.\n";
		}
		return false;
	}

	bool removeDirectory(std::string machineName, std::string machineID)
	{
		std::string folderName = machineName + "_" + machineID;
		folderName = ".\\Machines\\" + folderName;
		// Check if the folder already exists
		if (fs::exists(folderName)) {
			// Create the directory
			if (fs::remove_all(folderName)) {
				std::cout << "\nFolder Deleted successfully!\n" << std::endl;
				return true;
			}
			else {
				std::cout << "\nFailed to Remove folder.\n" << std::endl;
			}
		}
		else {
			std::cout << "\nFolder does not exist.\n" << std::endl;
		}
		return false;
	}

	//only deletes the folders created in the .\Machines
	void deleteAllDirectory() {
		MachineNode* tempIterator = MHead;
		while (tempIterator->getNext() != MHead) {
			removeDirectory(tempIterator->getMachineName(), tempIterator->getMachineID());
			tempIterator = tempIterator->getNext();
		}
		removeDirectory(tempIterator->getMachineName(), tempIterator->getMachineID());
	}

	bool machineAdd(std::string Mname)
	{
		if (totalMachines >= UFO::subtract(identifierSpace, "1"))
			return false;

		std::string identifier;
		if (!manual)
		{
			identifier = sha1(Mname);
			identifier = UFO::truncateHash(identifier);
		}
		else {
			std::cout << "\nSettings are Manual. Please enter Machine ID as well: ";
			std::cin >> identifier;
			std::cin.ignore();
			while ((identifierSpace.length() == identifier.length() && identifier >= identifierSpace) || identifier.length() > identifierSpace.length()) {
				std::cout << "\nId out of bound. Please enter valid ID: ";
				std::cin >> identifier;
				std::cin.ignore();
			}
			identifier = UFO::truncateHash(identifier); // to add zeros if there arnt.
			while (identifier > UFO::subtract(identifierSpace, "1") || identifier < UFO::truncateHash("0000000000000000000000000000000000000000") || ((identifierSpace.length() == identifier.length() && identifier >= identifierSpace) || identifier.length() > identifierSpace.length())) {
				std::cout << "\nId out of bound. Please enter valid ID: ";
				std::cin >> identifier;
				std::cin.ignore();
				if (identifierSpace.length() == identifier.length() && identifier > identifierSpace)
					continue;
				identifier = UFO::truncateHash(identifier);
			}
		}

		if (MHead == nullptr)
		{
			MHead = new MachineNode(identifier, Mname, MHead, identifierSpaceInt, orderOfBTree);
			MHead->setNext(MHead);
			totalMachines = UFO::addHex(totalMachines, "1");

			createDirectory(Mname, identifier);
			return true;
		}
		else
		{
			if (MHead->getMachineID() == identifier)
			{
				std::cout << "Same Machine, not adding\t" << Mname << std::endl;
				return false;
			}

			MachineNode* tempIterator = MHead;
			while (tempIterator->getNext() != MHead)
			{
				if (tempIterator->getMachineID() == identifier)
				{
					std::cout << "Same Machine, not adding\t" << Mname << std::endl;
					return false;
				}
				tempIterator = tempIterator->getNext();
			}
			if (tempIterator->getMachineID() == identifier)
			{
				std::cout << "Same Machine, not adding\t" << Mname << std::endl;
				return false;
			}

			if (MHead->getMachineID() > identifier)
			{
				MachineNode* tempHead = MHead;
				MachineNode* newNode;

				//MHead = newNode;

				newNode = new MachineNode(identifier, Mname, tempHead, identifierSpaceInt, orderOfBTree);

				if (tempHead->getNext() == tempHead)
				{
					tempHead->setNext(newNode);
					MHead = newNode;
				}
				else
				{
					MachineNode* tempIterator = tempHead;
					while (tempIterator->getNext() != tempHead)
					{
						tempIterator = tempIterator->getNext();
					}

					MHead = newNode;
					tempIterator->setNext(MHead);
				}
				totalMachines = UFO::addHex(totalMachines, "1");

				// Create a directory for this machine so that the reassign files function can add the required files to the directory
				createDirectory(Mname, identifier);
				reassignFilesAdd(newNode);
				return true;
			}
			else
			{
				MachineNode* tempIterator = MHead;
				while (tempIterator->getNext()->getMachineID() < identifier)
				{
					if (tempIterator->getNext() == MHead)
					{
						break;
					}

					tempIterator = tempIterator->getNext();
				}

				MachineNode* tempNext = tempIterator->getNext();
				MachineNode* newNode = new MachineNode(identifier, Mname, tempNext, identifierSpaceInt, orderOfBTree);
				tempIterator->setNext(newNode);

				// Create a directory for this machine so that the reassign files function can add the required files to the directory
				createDirectory(Mname, identifier);
				reassignFilesAdd(newNode);

				// Note: The newNode pointer is being initialized with the Wrong structure which would lead to the wrong construction of RFT
				// But this issue would be addressed because we are reconstructing the RFTs for all machineNodes
				totalMachines = UFO::addHex(totalMachines, "1");
				return true;
			}

		}
		return true;
	}

	bool machineRemove(std::string mName)
	{

		if (totalMachines == UFO::truncateHash("0000000000000000000000000000000000000000"))
		{
			return false;
		}

		if (MHead == MHead->getNext() && MHead->getMachineName() == mName)
		{
			if (mName == MHead->getMachineName())
			{
				//just delete the folde, no need to reassign files since there was only a singe machine left in the system
				removeDirectory(mName, MHead->getMachineID());
				delete MHead;
				MHead = nullptr;

				totalMachines = UFO::subtract(totalMachines, "1");

				return true;
			}
			else
			{
				std::cout << "\nThe Machine Name provided is invalid. Please enter a valid name!\n";
				return false;
			}
		}

		MachineNode* tempIterator = MHead;
		MachineNode* tempPrevious = MHead;

		while (tempIterator->getMachineName() != mName)
		{
			tempPrevious = tempIterator;
			tempIterator = tempIterator->getNext();

			if (tempIterator == MHead)
			{
				std::cout << "\nThe Machine Name provided is invalid. Please enter a valid name!\n";
				return false;
			}
		}


		reassignFilesDel(tempIterator); // first push files to the next machine, and then its deleted from the system
		removeDirectory(mName, tempIterator->getMachineID());

		// Now we know that tempIterator is the node that is to be removed. tempIterator->getNext() is it's successor
		if (tempPrevious == tempIterator)
		{
			while (tempPrevious->getNext() != tempIterator)
			{
				tempPrevious = tempPrevious->getNext();
			}
			MHead = tempIterator->getNext();
		}

		// Now we have the previous node of the tempIterator for all cases
		tempPrevious->setNext(tempIterator->getNext());


		delete tempIterator;
		return true;

	}

	void reconstructRFTs()
	{
		MachineNode* tempIterator = MHead;

		while (tempIterator->getNext() != MHead)
		{
			tempIterator->reconstructRFT(MHead);
			tempIterator = tempIterator->getNext();
		}

		tempIterator->reconstructRFT(MHead);

		return;
	}

	void printing()
	{
		MachineNode* tempIterator = MHead;

		if (MHead->getNext() == MHead)
		{
			std::cout << MHead->getMachineName() << std::endl;
			return;
		}

		while (tempIterator->getNext() != MHead)
		{
			std::cout << tempIterator->getMachineName() << std::endl;
			tempIterator = tempIterator->getNext();
		}

		std::cout << tempIterator->getMachineName() << std::endl;

	}

	void printRFTs()
	{
		MachineNode* tempIterator = MHead;

		while (tempIterator->getNext() != MHead)
		{
			tempIterator->printRFT();
			std::cout << std::endl << std::endl;
			tempIterator = tempIterator->getNext();
		}

		tempIterator->printRFT();
	}


	bool fileAdd(std::string filePath, std::string machineName = "", std::string manualID = "")
	{
		if (machineName == "")
		{
			machineName = MHead->getMachineName();
		}

		std::string machineHash;
		MachineNode* tempIterator = MHead;
		if (!manual)
			machineHash = sha1(machineName);
		else
			machineHash = manualID;

		machineHash = UFO::truncateHash(machineHash);

		tempIterator = tempIterator->search(machineHash);

		if (tempIterator->getMachineName() != machineName)
		{
			std::cout << "Please enter a valid machine name! The machine \"" << machineName << "\" does not exist.\n";
			return false;
		}

		// At this point , tempIterator points to the machine from which the query was generated

		std::ifstream fileToAdd(filePath);
		std::string fileContent = "";
		std::string fileHash;

		if (!fileToAdd)
		{
			std::cout << "\nThe file path you have provided is invalid!\n";
			return false;
		}

		std::ostringstream ss;

		ss << fileToAdd.rdbuf();
		fileContent = ss.str();

		fileHash = sha1(fileContent);
		fileHash = UFO::truncateHash(fileHash);

		fileToAdd.close();

		std::cout << "Hash for your file: " << fileHash << "\n";

		tempIterator = tempIterator->search(fileHash);
		// At this point, tempIterator points to the machine where the file is to be stored

		std::string newFilePath = ".\\Machines\\" + tempIterator->getMachineName() + "_" + tempIterator->getMachineID() + "\\" + UFO::removeNameExtension(UFO::extractNameFromPath(filePath));
		newFilePath = newFilePath + "_" + fileHash + ".txt";
		newFilePath = UFO::fixFilePath(newFilePath);

		std::ofstream os(newFilePath);

		if (os.is_open())
		{
			os << fileContent;
		}
		else
		{
			std::cout << UFO::extractNameFromPath(filePath) << " file did not open!\n";
		}

		tempIterator->fileAdd(newFilePath, fileHash);


		return true;
	}

	//called when a machine is added to pull files from the next machine
	void reassignFilesAdd(MachineNode* newlyAdded) {
		newlyAdded->pullFiles();
	}

	//called when machine is deleted to push machines to the next machine.
	void reassignFilesDel(MachineNode* newlyAdded) {
		newlyAdded->pushFiles();
	}



	void fileSearch(std::string fileKey, std::string  machineName = "") {
		//Now In DHT, First We will Find the Machine 
		if (machineName == "")
		{
			machineName = MHead->getMachineName();
		}

		MachineNode* tempIterator = MHead;
		std::string machineHash;
		if (!manual)
		{
			machineHash = sha1(machineName);
			machineHash = UFO::truncateHash(machineHash);
		}
		else {
			std::cout << "System is Manual. Please Enter Machine ID as well: ";
			getline(std::cin, machineHash);
			while (machineHash > UFO::subtract(identifierSpace, "1") || machineHash < UFO::truncateHash("0000000000000000000000000000000000000000") || ((identifierSpace.length() == machineHash.length() && machineHash >= identifierSpace) || machineHash.length() > identifierSpace.length())) {
				std::cout << "Out of Bound. Please Enter Valid ID: ";
				getline(std::cin, machineHash);
				if ((identifierSpace.length() == machineHash.length() && machineHash >= identifierSpace) || machineHash.length() > identifierSpace.length()) {
					continue;
				}
				machineHash = UFO::truncateHash(machineHash);
			}
			machineHash = UFO::truncateHash(machineHash);
		}


		tempIterator = tempIterator->search(machineHash);

		if (tempIterator->getMachineName() != machineName)
		{
			std::cout << "Please enter a valid machine name! The machine \"" << machineName << "\" does not exist.\n";
			return;
		}

		tempIterator = tempIterator->search(fileKey);

		tempIterator->fileSearch(fileKey);

	}

	void fileRemove(std::string fileKey, std::string  machineName = "") {
		//Now In DHT, First We will Find the Machine 
		if (machineName == "")
		{
			machineName = MHead->getMachineName();
		}

		MachineNode* tempIterator = MHead;
		std::string machineHash;
		if (!manual)
		{
			machineHash = sha1(machineName);
			machineHash = UFO::truncateHash(machineHash);
		}
		else {
			std::cout << "System is Manual. Please Enter Machine ID as well: ";
			getline(std::cin, machineHash);
			while (machineHash > UFO::subtract(identifierSpace, "1") || machineHash < UFO::truncateHash("0000000000000000000000000000000000000000") || ((identifierSpace.length() == machineHash.length() && machineHash >= identifierSpace) || machineHash.length() > identifierSpace.length())) {
				std::cout << "Out of Bound. Please Enter Valid ID: ";
				getline(std::cin, machineHash);
				if ((identifierSpace.length() == machineHash.length() && machineHash >= identifierSpace) || machineHash.length() > identifierSpace.length()) {
					continue;
				}
				machineHash = UFO::truncateHash(machineHash);
			}
			machineHash = UFO::truncateHash(machineHash);
		}

		tempIterator = tempIterator->search(machineHash);

		if (tempIterator->getMachineName() != machineName)
		{
			std::cout << "Please enter a valid machine name! The machine \"" << machineName << "\" does not exist.\n";
			return;
		}

		tempIterator = tempIterator->search(fileKey);

		tempIterator->fileRemove(fileKey, 0);
	}

	void generateDot() {

		int currentnodeIndex = 0;
		MachineNode* current = MHead;

		std::ofstream dotFile("DHT.dot");
		dotFile << "digraph CircularLinkedList { node[shape=record]; rankdir=LR; edge[tailclip=false,arrowtail=dot,dir=both];" << std::endl;


		while (current->getNext()->getMachineID() != MHead->getMachineID()) {


			dotFile << "p" << currentnodeIndex << "[label=\"{" << "<data>" << current->getMachineName() << "|" << "<data>" << current->getMachineID() << "|" << "<next>" << "}\"];\n";

			dotFile << "p" << currentnodeIndex << ":next:c -> p" << currentnodeIndex + 1 << ":data;\n";

			current = current->getNext();
			currentnodeIndex++;
		}
		//Now Add One Here as Well

		dotFile << "p" << currentnodeIndex << "[label=\"{" << "<data>" << current->getMachineName() << "|" << "<data>" << current->getMachineID() << "|" << "<next>" << "}\"];\n";
		dotFile << "p" << currentnodeIndex << ":next:c -> p0:data[constraint=false];\n";
		dotFile << "}" << std::endl;

		dotFile.close();

		std::string command = "dot -Tpng -O DHT.dot";

		// Execute the command
		int result = std::system(command.c_str());

		if (result == 0) {
			std::cout << "Graph generated successfully." << std::endl;
			std::system("start DHT.dot.png");
		}
		else {
			std::cerr << "Error generating graph." << std::endl;
		}

	}
	void printBTree() {
		std::string MacName;
		std::cout << "\nEnter Name of Machine: ";
		std::getline(std::cin, MacName);
		MachineNode* tempIterator = MHead;
		while (tempIterator->getNext() != MHead) {
			if (tempIterator->getMachineName() == MacName)
				break;
			tempIterator = tempIterator->getNext();
		}
		if (tempIterator->getMachineName() == MacName) {
			tempIterator->getBTreeHead()->generateBTreeDotFile();
		}
		else {
			std::cout << "\nMachine could not be found!\n";
		}
	}
	void printRoutingTable() {
		std::string MacName;
		std::cout << "\nEnter Name of Machine: ";
		std::getline(std::cin, MacName);
		MachineNode* tempIterator = MHead;
		while (tempIterator->getNext() != MHead) {
			if (tempIterator->getMachineName() == MacName)
				break;
			tempIterator = tempIterator->getNext();
		}
		if (tempIterator->getMachineName() == MacName) {
			tempIterator->generateDotRFT();
		}
		else {
			std::cout << "\nMachine could not be found!\n";
		}
	}
};







class IPFS
{
private:
	DistributedHashTable* CircularHashTable;

	int identifierSpaceInBits;
	int orderOfBTree;



public:
	IPFS()
	{
		askIdentifierSpace();
		askForOrderOfBtree();
		CircularHashTable = new DistributedHashTable(orderOfBTree, identifierSpaceInBits);
		askForManual();
		askInitialMachines();
	}
	// Note: We would not give the user any access to our main classes.
	// The user would only have access to the IPFS class and the function calls would be generated from here.

	void askInitialMachines() {
		std::cout << "Specify Initial Number of Machines: ";
		int numMachines;
		std::cin >> numMachines;
		std::cin.ignore();

		for (int i = 0; i < numMachines; i++)
		{
			machineAdd();
		}

	}

	void askForManual() {
		char choice;
		std::cout << "Do you want to manually assign IDs to your Machines? (This setting would be persistent throughout the program) Y/N: ";
		std::cin >> choice;

		while (choice != 'N' && choice != 'n' && choice != 'Y' && choice != 'y') {
			std::cout << "Invalid choice. Choose Y/y or N/n: ";
			std::cin >> choice;
			if (choice == 'Y' || choice == 'y')
			{
				CircularHashTable->setManual();
				break;
			}
		}
		if (choice == 'Y' || choice == 'y')
			CircularHashTable->setManual();


	}

	// Displays the user interface for all operations in the IPFS. Generates the function calls of the other classes. 
	void displayMenu()
	{
		char LC = 201;
		char RC = 187;
		char BLC = 200;
		char BRC = 188;
		char H = 205;
		char V = 186;

		std::cout << "\n";
		std::cout << LC << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << RC << std::endl;
		std::cout << V << "                   _________________ _____                    " << V << std::endl;
		std::cout << V << "....o o o O O O O |_   _| ___ \\  ___/  ___| O O O O o o o ...." << V << std::endl;
		std::cout << V << "....o o o O O O O   | | | |_/ / |_  \\ `--.  O O O O o o o ...." << V << std::endl;
		std::cout << V << "....o o o O O O O   | | |  __/|  _|  `--. \\ O O O O o o o ...." << V << std::endl;
		std::cout << V << "....o o o O O O O   | |_| |   | |   /\\__/ / O O O O o o o ...." << V << std::endl;
		std::cout << V << "....o o o O O O O  \\___/\\_|   \\_|   \\____/  O O O O o o o ...." << V << std::endl;
		std::cout << V << "                                                              " << V << std::endl;
		std::cout << V << "    0) Retrieve Existing File.                                " << V << std::endl;
		std::cout << V << "    1) Add a machine to filing system.                        " << V << std::endl;
		std::cout << V << "    2) Remove a machine from filing system.                   " << V << std::endl;
		std::cout << V << "    3) Send a file addition query.                            " << V << std::endl;
		std::cout << V << "    4) Send a file removal query.                             " << V << std::endl;
		std::cout << V << "    5) Print BTree Of Any Machine.                            " << V << std::endl;
		std::cout << V << "    6) Print Routing Table Of Any Machine.                    " << V << std::endl;
		std::cout << V << "    7) Print DHT.                                             " << V << std::endl;
		std::cout << V << "    8) Remove All Directories.                                " << V << std::endl;
		std::cout << V << "    9) Exit the System.                                       " << V << std::endl;
		std::cout << V << "                                                              " << V << std::endl;
		std::cout << BLC << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << H << BRC << std::endl;

	}

	void interface()
	{
		int choice;
		while (true)
		{
			displayMenu();
			std::cout << "\nPlease make a decision: ";
			std::cin >> choice;
			std::cin.ignore();

			if (choice == 0) {
				fileSearch();
				continue;
			}
			else if (choice == 1) {
				machineAdd();
				continue;
			}
			else if (choice == 2) {
				machineRemove();
				continue;
			}
			else if (choice == 3) {
				fileAdd();
				continue;
			}
			else if (choice == 4) {
				fileRemove();
				continue;
			}
			else if (choice == 5) {
				printBTree();
				continue;
			}
			else if (choice == 6) {
				printRoutingTable();
				continue;
			}
			else if (choice == 7) {
				printDHT();
				continue;
			}
			else if (choice == 8) {
				CircularHashTable->deleteAllDirectory();
				continue;
			}
			else if (choice == 9) {
				break;
			}
			else {
				std::cout << "Invalid Choice.";
				continue;
			}
		}


	}

	// Asks the user for the identifier space (i.e Number of bits). This identifier space would dictate the scope of the machine nodes and files.
	// If user enters 5, the identifier space would be 2^5.
	void askIdentifierSpace()
	{
		std::cout << "Please enter the identifier space (in bits) of the InterPlanetary Filing System: ";
		std::cin >> identifierSpaceInBits;
	}

	// Iterates the Circular DHT and finds the specific file in the BTree, against which the key was generated. (This key would be displayed to the user at the point of insertion.)
	// Searching would be done according to the FT Tables and the rules predetermined in the pdf.
	//std::string search();

	// 
	void fileAdd()
	{
		std::string filePath;
		std::string machineName;

		std::cout << "\nEnter the machine where the file add query is being sent: ";
		std::getline(std::cin, machineName);

		std::string machineID = "";

		if (CircularHashTable->getManual())
		{
			std::cout << "\nID system is manual. Please enter the machine ID: ";
			std::getline(std::cin, machineID);

			//big change
			machineID = UFO::truncateHash(machineID);
		}

		std::cout << "\nEnter the path of the file you want to store: ";
		std::getline(std::cin, filePath);

		CircularHashTable->fileAdd(filePath, machineName, machineID);
	}

	void machineAdd()
	{
		std::string machineName;

		std::cout << "\nEnter the name of the machine you want to add: ";
		std::getline(std::cin, machineName);

		if (CircularHashTable->machineAdd(machineName))
		{
			CircularHashTable->reconstructRFTs();
		}

	}

	void machineRemove()
	{
		std::string machineName;

		std::cout << "\nEnter the name of the machine you want to remove: ";
		std::getline(std::cin, machineName);

		bool removed = false;

		removed = CircularHashTable->machineRemove(machineName);

		//if (removed)
		//{
		//	// remove the directory
		//}
	}

	void askForOrderOfBtree()
	{
		std::cout << "Please enter the order of the BTrees of each machine: ";
		std::cin >> orderOfBTree;
	}

	void fileSearch() {

		std::string fileKey;
		std::string machineName;

		std::cout << "\nEnter the machine where the file Search query is being Initiated: ";
		std::getline(std::cin, machineName);

		std::cout << "\nEnter the Key of the file you want to Search: ";
		std::getline(std::cin, fileKey);

		CircularHashTable->fileSearch(fileKey, machineName);

	}

	void fileRemove() {

		std::string fileKey;
		std::string machineName;

		std::cout << "\nEnter the machine where the file Remove query is being Initiated: ";
		std::getline(std::cin, machineName);

		std::cout << "\nEnter the Key of the file you want to Remove: ";
		std::getline(std::cin, fileKey);

		CircularHashTable->fileRemove(fileKey, machineName);

	}
	void printBTree() {
		CircularHashTable->printBTree();
	}
	void printRoutingTable() {
		CircularHashTable->printRoutingTable();
	}
	void printDHT() {
		CircularHashTable->generateDot();
	}
};