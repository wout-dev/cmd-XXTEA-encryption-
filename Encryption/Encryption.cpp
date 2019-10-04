
#include "stdafx.h"
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>
#include <stdlib.h>

#define DELTA 0x9e3779b9

#define MX (((z>>5^y<<2) + ( y >>3^z << 4 )) ^ ((sum^y) + (key[(p&3)^e]^z)))


long btea(uint32_t *v, int n, uint32_t const key[4]) {
	unsigned long z = v[n - 1], y = v[0], sum = 0, e;
	long p, q;
	if (n > 1) {          /* Coding Part */
		q = 6 + 52 / n;
		while (q-- > 0) {
			sum += DELTA;
			e = (sum >> 2) & 3;
			for (p = 0; p<n - 1; p++) y = v[p + 1], z = v[p] += MX;
			y = v[0];
			z = v[n - 1] += MX;
		}
		return 0;
	}
	else if (n < -1) {  /* Decoding Part */
		n = -n;
		q = 6 + 52 / n;
		sum = q * DELTA;
		while (sum != 0) {
			e = (sum >> 2) & 3;
			for (p = n - 1; p>0; p--) z = v[p - 1], y = v[p] -= MX;
			z = v[n - 1];
			y = v[0] -= MX;
			sum -= DELTA;
		}
		return 0;
	}
	return 1;
}


void encryption(int argc, char ** argv)
{
	enum OperatingMode
	{
		encrypt,
		decrypt
	};
	/* Debug
	std::cout << "you have entered" << argc << "arguments: " << "\n";
	for (int i = 0; i < argc; ++i)
	{
		std::cout << "_" <<argv[i] << "_"<< "\n";
	}
	std::cout << std::endl; 
	std::cout << argv[0] << std::endl; 
	*/
	int offset = 1; 
	auto checkifCMD = argv[0]; 
	if (checkifCMD[0] == 'C' && checkifCMD[1] == ':')
	{ //Supports launching with set arguments via vs or other script
		std::cout << "is not from cmd" << std::endl; 
		offset = 0;
	}
	if (argc - offset < 1)
	{
		std::cout << "please enter enough arguments" << std::endl; 
		return;
	}
	auto const modestr = argv[2 - offset];
	auto const filepath = argv[3 - offset];
	auto password = argv[4 - offset];

	
	//This cast is needed to make this comparision work in my compiler (.16.x). no need to cast if using a different compiler (like gcc or clang)
	if (static_cast<std::string>(modestr) != "encrypt" && static_cast<std::string>(modestr) != "decrypt")
	{
		std::cout << "first arg: " << modestr << std::endl;;
		std::cout << "please enter encrypt or decrypt as first keyword" << std::endl;
		return ;
		//std::cin.ignore();
	}
	OperatingMode mode = OperatingMode::encrypt;
	if (static_cast<std::string>(modestr) == "decrypt")
		mode = OperatingMode::decrypt;
	if (strlen(password) > 16)
	{
		std::cout << "max password size exceeded" << std::endl; 
		return; 
	}
	uint32_t  keys[4];
	memcpy(keys, password, strlen(password)); //Keys


	std::ifstream fs; 
	fs.open(static_cast<std::string>(filepath));
	if (!fs.is_open())
	{
		std::cout << "faled to open file: " << filepath << std::endl;
		return ;
	}

	//Encryption 
	//Initial size n
	//Adjust size newsize
	if (mode == OperatingMode::encrypt)
	{
		std::vector<char> ToBeEncrypted;
		while (!fs.eof())
		{
			ToBeEncrypted.push_back(fs.get()); //load data
		}
		ToBeEncrypted.pop_back(); 
		int const n = ToBeEncrypted.size(); //Size of data 
		int newsize = n; //Adjusted size 
		int const paddingsize = 4 - (n % 4);
		newsize = n + paddingsize;
		for (int i = 0; i < paddingsize - 1; i++) //Random padding
			ToBeEncrypted.push_back(static_cast<char>(rand() % 126));
		if (paddingsize > 0) //push padddingsize at back
			ToBeEncrypted.push_back(static_cast<char>(paddingsize));

		btea((uint32_t*)&ToBeEncrypted[0], newsize / 4, keys); 	//Encrypt

		//TODO OUTPUT:
		std::ofstream ofstreamFile;
		ofstreamFile.open(filepath);
		if (ofstreamFile.is_open())
		{
			for (auto i = 0; i < newsize; i++)
				ofstreamFile << ToBeEncrypted[i];
		}
			
		else
			std::cout << "failed to open output path :  " << filepath << std::endl; 
		ofstreamFile.close();

	}
	else
	{
		std::vector<char> ToBeEncrypted;
		while (!fs.eof())
		{
			ToBeEncrypted.push_back(fs.get()); //load data
		}
		ToBeEncrypted.pop_back();
		int const newsize = ToBeEncrypted.size();
		btea((uint32_t*)&ToBeEncrypted[0], -newsize / 4, keys);	//Decrypt
		auto padsize = static_cast<int>(ToBeEncrypted[newsize - 1]);
		for (int i = 0; i < newsize - padsize ; i++) 	//Print decrypted 
			std::cout << i + 1 << ":" << ToBeEncrypted[i] << " ";
	
		std::ofstream ofstreamFile;
		ofstreamFile.open(filepath);
		if (ofstreamFile.is_open())
		{
			for (auto i = 0; i < newsize - padsize; i++)
				ofstreamFile << ToBeEncrypted[i];
		}
		else
			std::cout << "failed to open output path :  " << filepath << std::endl;
		ofstreamFile.close();
	}
	return ;
}
/**/
int main(int argc, char ** argv)
{
	encryption(argc, argv);


}

