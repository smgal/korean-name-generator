
#ifndef __KOREAN_NAMER_H__
#define __KOREAN_NAMER_H__

////////////////////////////////////////////////////////////////////////////////
// uses

#include <string>
#include <vector>
#include <map>
#include <cassert>

////////////////////////////////////////////////////////////////////////////////
// interface / implementation

class KoreanNamer
{
	enum
	{
		_ORDER = 3
	};

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
	static KoreanNamer* create(const std::vector<std::string>& texts)
	{
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

				char ch = (j + _ORDER < p_text->size()) ? (*p_text)[j + _ORDER] : ' ';

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

	std::string nameSomeone(void) const
	{
		// TODO: ...
		return "";
	}

private:
	KoreanNamer()
		: _num_ngrams(0)
	{
	}

private:
	int _num_ngrams;
	std::map<std::string, Ngram> _ngram_map;

};

#endif // __KOREAN_NAMER_H__
