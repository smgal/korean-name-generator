
#ifndef __KOREAN_NAMER_H__
#define __KOREAN_NAMER_H__

////////////////////////////////////////////////////////////////////////////////
// uses

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cassert>

////////////////////////////////////////////////////////////////////////////////
// interface / implementation

class KoreanNamer
{
	static const int  _ORDER = 3;
	static const char _DELIMITER = '$';
	static const int  _MAX_LEN = 100;

	struct Ngram
	{
		bool is_head;
		int  count;
		std::map<char, int> id_map;

		Ngram(bool _is_forefont)
			: is_head(_is_forefont)
			, count(0)
		{}
	};

public:
	static KoreanNamer* create(const std::vector<std::string>& _texts)
	{
		std::vector<std::string> texts;
		{
			std::for_each(_texts.begin(), _texts.end(), [&texts](const std::string k_str)
			{
				texts.push_back(_convKorToEng(k_str));
			});
		}

		KoreanNamer* p_inst = new KoreanNamer;

		auto p_text = texts.begin();
		for (; p_text != texts.end(); ++p_text)
		{
			if (p_text->size() < _ORDER)
				continue;

			for (int j = 0; j < p_text->size() - (_ORDER - 1); j++)
			{
				std::string str = p_text->substr(j, _ORDER);
				auto p_ngram_map = p_inst->_ngram_map.find(str);

				if (p_ngram_map == p_inst->_ngram_map.end())
				{
					p_inst->_ngram_map.insert(std::make_pair(p_text->substr(j, _ORDER), Ngram(j == 0)));
					p_inst->_num_ngrams += (j == 0) ? 1 : 0;
					p_ngram_map = p_inst->_ngram_map.find(str);
					assert(p_ngram_map != p_inst->_ngram_map.end());
				}

				char ch = (j + _ORDER < p_text->size()) ? (*p_text)[j + _ORDER] : _DELIMITER;

				auto p_char_map = p_ngram_map->second.id_map.find(ch);
				if (p_char_map != p_ngram_map->second.id_map.end())
					p_char_map->second++;
				else
					p_ngram_map->second.id_map.insert(std::make_pair(ch, 1));

				p_ngram_map->second.count++;
			}
		}

		return p_inst;
	}

	std::string nameSomeone(int (*fn_getRandom)(int n)) const
	{
		std::string result = "";

		{
			int match_count = fn_getRandom(_num_ngrams);

			auto p_ngram_map = _ngram_map.begin();
			for (; p_ngram_map != _ngram_map.end(); p_ngram_map++)
			{
				if (p_ngram_map->second.is_head && match_count-- == 0)
				{
					result += p_ngram_map->first;
					break;
				}
			}

			assert(result.size() > 0);
		}

		while (*result.rbegin() != _DELIMITER)
		{
			auto p_ngram_map = _ngram_map.find(result.substr(result.size() - _ORDER));
			assert(p_ngram_map != _ngram_map.end());

			int matched = fn_getRandom(_num_ngrams);
			int count = 0;

			auto p_char_map = p_ngram_map->second.id_map.begin();
			for (; p_char_map != p_ngram_map->second.id_map.end(); p_char_map++)
			{
				count += p_char_map->second;
				if (count >= matched)
				{
					result += p_char_map->first;
					break;
				}
			}

			if (result.size() >= _MAX_LEN)
				break;
		}

		assert(result.size() > 0);
		assert(*result.rbegin() == _DELIMITER);

		return _convEngToKor(result.substr(0, result.size() - 1));
	}

private:
	KoreanNamer()
		: _num_ngrams(0)
	{
	}

private:
	int _num_ngrams;
	std::map<std::string, Ngram> _ngram_map;

	static std::string _convKorToEng(const std::string& kor)
	{
/*
		std::string result;

		if ((kor[0] & 0xF0) == 0xE0 && (kor[1] & 0xC0) == 0x80 && (kor[2] & 0xC0) == 0x80)
		{
			typedef unsigned int uint;
			uint code = uint(kor[0] & 0x0F) << 12 | uint(kor[1] & 0x3F) << 6 | uint(kor[2] & 0x3F);

			if (code >= 0xAC00 && code <= 0xD7A3)
			{
				std::vector<std::string> QQ[3] =
				{
					{ "r","R","s","e","E","f","a","q","Q","t","T","d","w","W","c","z","x","v","g" },
					{ "k","o","i","O","j","p","u","P","h","hk","ho","hl","y","n","nj","np","nl","b","m","ml","l" },
					{ " ","r","R","rt","s","sw","sg","e","f","fr","fa","fq","ft","fx","fv","fg","a","q","qt","t","T","d","w","c","z","x","v","g" }
				};

				// ぁあいぇえぉけげこさざしじすずせぜそぞ
				const int MAX_SM1 = 19;
				// ただちぢっつづてでとどなにぬねのはばぱひび
				const int MAX_SM2 = 21;
				//   ぁあぃいぅうぇぉおかがきぎくぐけげごさざしじずせぜそぞ
				const int MAX_SM3 = 28;

				code -= 0xAC00;

				uint SM1 = code / (MAX_SM2 * MAX_SM3);
				uint SM2 = (code - SM1 * (MAX_SM2 * MAX_SM3)) / MAX_SM3;
				uint SM3 = code - SM1 * (MAX_SM2 * MAX_SM3) - SM2 * MAX_SM3;

				result += QQ[0][SM1];
				result += QQ[1][SM2];
				result += QQ[2][SM3];
			}
		}
*/
		return kor;
	}

	static std::string _convEngToKor(const std::string& eng)
	{
		return eng;
	}

};

#endif // __KOREAN_NAMER_H__
