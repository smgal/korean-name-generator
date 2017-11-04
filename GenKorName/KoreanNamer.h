
#ifndef __KOREAN_NAMER_H__
#define __KOREAN_NAMER_H__

////////////////////////////////////////////////////////////////////////////////
// uses

#include <string>

////////////////////////////////////////////////////////////////////////////////
// interface / implementation

class KoreanNamer
{
public:
	static KoreanNamer* create(const std::string& text_file_name)
	{
		// TODO: ...
		return nullptr;
	}

	std::string nameSomeone(void) const
	{
		// TODO: ...
		return "";
	}

private:
	KoreanNamer();
};

#endif // __KOREAN_NAMER_H__
