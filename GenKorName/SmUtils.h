
#ifndef __SM_UTILS_H__
#define __SM_UTILS_H__

////////////////////////////////////////////////////////////////////////////////
// uses

#include <string>
#include <vector>
#include <random>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
// interface / implementation

namespace sm
{
	std::vector<std::string> makeStringListFromFile(const std::string& file_path)
	{
		std::vector<std::string> result;

		FILE* file = fopen(file_path.c_str(), "r");

		if (file)
		{
			{
				bool verified = false;

				fseek(file, 0, SEEK_END);
				int file_len = ftell(file);
				fseek(file, 0, SEEK_SET);

				if (file_len >= 4)
				{
					unsigned char BOM_ID[4];
					fread(BOM_ID, 1, sizeof(BOM_ID), file);
					fseek(file, 0, SEEK_SET);

					// EF BB BF -> UTF-8
					if (BOM_ID[0] == 0xEF && BOM_ID[1] == 0xBB && BOM_ID[2] == 0xBF)
					{
						fseek(file, 3, SEEK_SET);
						verified = true;
					}

					// 00 00 FE FF	-> UTF-32(BE)
					// FF FE 00 00	-> UTF-32(LE)
					// FE FF -> UTF-16(BE)
					// FF FE -> UTF-16(LE)
				}

				if (!verified)
				{
					fclose(file);
					return std::vector<std::string>();
				}
			}

			std::string s;
			int ch;

			while ((ch = fgetc(file)) != EOF)
			{
				if (ch == ' ' || ch == '\n' || ch == '\r')
				{
					if (s.size() > 0)
						result.push_back(s);

					s.clear();
					continue;
				}

				s += ch;
			}

			if (s.size() > 0)
				result.push_back(s);

			fclose(file);
		}

		return result;
	}

	int getRandom(int n)
	{
		std::random_device rand_device;
		std::mt19937 rand_gen(rand_device());
		std::uniform_int_distribution<int> distrib(0, n - 1);
		return distrib(rand_gen);
	}
}

#endif // #ifndef __SM_UTILS_H__
