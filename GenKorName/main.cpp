
#include "KoreanNamer.h"
#include "SmUtils.h"

#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include <memory>

namespace
{
	const char* NAME_LIST_FILE = "korean_name_to_be_learned.txt";
	const int   NUM_RESULTS = 10;
}

int main(void)
{
	std::vector<std::string> text_list = sm::makeStringListFromFile(NAME_LIST_FILE);

	std::unique_ptr<KoreanNamer> namer(KoreanNamer::create(text_list));

	assert(namer);

	for (int i = 0; i < NUM_RESULTS; i++)
		printf("[%2d] %s\n", i, namer->nameSomeone().c_str());

#if defined(_MSC_VER)
	system("pause");
#endif

	return 0;
}
