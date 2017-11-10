
#include "KoreanNamer.h"
#include "SmUtils.h"

#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <memory>

#if defined(_MSC_VER)
#include <Windows.h>
#endif

namespace
{
	const char* NAME_LIST_FILE = "korean_name_to_be_learned.txt";
	//const char* NAME_LIST_FILE = "fantasy_name_to_be_learned.txt";
	const int   NUM_RESULTS = 100;
}

int _getTestRandom(int n)
{
	return n-1;
}

int main(void)
{
	std::vector<std::string> text_list = sm::makeStringListFromFile(NAME_LIST_FILE);

	std::unique_ptr<KoreanNamer> namer(KoreanNamer::create(text_list));

	assert(namer);

	for (int i = 0; i < NUM_RESULTS; i++)
	{
		std::string name = namer->nameSomeone(sm::getRandom);
		bool not_found = std::find(text_list.begin(), text_list.end(), name) == text_list.end();

#if defined(_MSC_VER)
		char    sz_utf8[256] = { 0, };
		wchar_t sz_usc2[256] = { 0, };
		char    sz_char[256] = { 0, };
		int     len = 0;

		strcpy_s(sz_utf8, 256, name.c_str());
		len = MultiByteToWideChar(CP_UTF8, 0, sz_utf8, strlen(sz_utf8), NULL, NULL);
		MultiByteToWideChar(CP_UTF8, 0, sz_utf8, strlen(sz_utf8), sz_usc2, len);

		len = WideCharToMultiByte(CP_ACP, 0, sz_usc2, -1, NULL, 0, NULL, NULL);
		WideCharToMultiByte(CP_ACP, 0, sz_usc2, -1, sz_char, len, NULL, NULL);

		printf("[%2d] %s %s\n", i, sz_char, not_found ? "(new)" : "");
#else
		printf("[%2d] %s %s\n", i, name.c_str(), not_found ? "(new)" : "");
#endif
	}

#if defined(_MSC_VER)
	system("pause");
#endif

	return 0;
}
