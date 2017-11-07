
#ifndef __KOREAN_NAMER_H__
#define __KOREAN_NAMER_H__

////////////////////////////////////////////////////////////////////////////////
// uses

#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// interface / implementation

class KoreanNamer
{

public:
	~KoreanNamer();

	static KoreanNamer* create(const std::vector<std::string>& texts);

	// @not_implemented
	static KoreanNamer* create(const std::string& packed_data_path) { return nullptr; }
	// @not_implemented
	bool savePackedData(const std::string& packed_data_path) const { return false; }

	std::string nameSomeone(int(*fn_getRandom)(int n)) const;

private:
	KoreanNamer();

private:
	struct _KoreanNamer* _p_impl;

};

#endif // __KOREAN_NAMER_H__
