
#ifndef __SM_UTILS_H__
#define __SM_UTILS_H__

////////////////////////////////////////////////////////////////////////////////
// uses

#include <string>
#include <vector>
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
}

#endif // #ifndef __SM_UTILS_H__
