
////////////////////////////////////////////////////////////////////////////////
// uses

#include "KoreanNamer.h"

#include <map>
#include <algorithm>
#include <cassert>

////////////////////////////////////////////////////////////////////////////////
// internal functions

namespace
{
	static const char _END_MARK = '$';
}

namespace
{
	typedef unsigned int uint;

	std::vector<std::string> SMGAL[3] =
	{
		{ "r","R","s","e","E","f","a","q","Q","t","T","d","w","W","c","z","x","v","g" },
		{ "k","o","i","O","j","p","u","P","h","hk","ho","hl","y","n","nj","np","nl","b","m","ml","l" },
		{ "_","r","R","rt","s","sw","sg","e","f","fr","fa","fq","ft","fx","fv","fg","a","q","qt","t","T","d","w","c","z","x","v","g" }
	};

	std::string _convKorToEng(const std::string& kor)
	{
		// Korean word only
		assert((kor.size() % 3) == 0);

		std::string result;

		const char* p_ch = kor.c_str();
		const char* p_ch_end = p_ch + kor.length();

		while (p_ch < p_ch_end)
		{
			assert((p_ch[0] & 0xF0) == 0xE0 && (p_ch[1] & 0xC0) == 0x80 && (p_ch[2] & 0xC0) == 0x80);

			uint code = uint(p_ch[0] & 0x0F) << 12 | uint(p_ch[1] & 0x3F) << 6 | uint(p_ch[2] & 0x3F);

			assert(code >= 0xAC00 && code <= 0xD7A3);

			{
				// ぁあいぇえぉけげこさざしじすずせぜそぞ
				const int MAX_SM1 = 19;
				// ただちぢっつづてでとどなにぬねのはばぱひび
				const int MAX_SM2 = 21;
				//   ぁあぃいぅうぇぉおかがきぎくぐけげごさざしじずせぜそぞ
				const int MAX_SM3 = 28;

				code -= 0xAC00;

				const uint SM1 = code / (MAX_SM2 * MAX_SM3);
				const uint SM2 = (code - SM1 * (MAX_SM2 * MAX_SM3)) / MAX_SM3;
				const uint SM3 = code - SM1 * (MAX_SM2 * MAX_SM3) - SM2 * MAX_SM3;

				result += SMGAL[0][SM1];
				result += SMGAL[1][SM2];
				result += SMGAL[2][SM3];
			}

			p_ch += 3;
		}

		return result;
	}

	std::string _convEngToKor(const std::string& eng)
	{
		std::string result;

		const char* p_ch = eng.c_str();
		const char* p_ch_end = p_ch + eng.length();

		uint SM1 = 0, SM2 = 0, SM3 = 0;
		int state = 0;

		while (p_ch < p_ch_end || state == 3)
		{
			static const char* JA_1ST = "rRseEfaqQtTdwWczxvg";
			static const char* JA_1ST_END = JA_1ST + strlen(JA_1ST);

			static const char* JA_2ND = "koiOjpuPhynbml";
			static const char* JA_2ND_END = JA_2ND + strlen(JA_2ND);

			switch (state)
			{
			case 0:
				{
					auto ix = std::find(JA_1ST, JA_1ST_END, *p_ch++);
					SM1 = ix - JA_1ST;
					state++;
				}
				break;
			case 1:
				{
					char ch2 = (p_ch + 1 < p_ch_end) ? *(p_ch + 1) : _END_MARK;

					auto ix1 = std::find(JA_2ND, JA_2ND_END, *p_ch++);
					auto ix2 = std::find(JA_2ND, JA_2ND_END, ch2);

					std::string s;
					s += ix1[0]; //
					if (ix2 < JA_2ND_END)
					{
						s += ix2[0];
						p_ch++;
					}

					auto ix = std::find(SMGAL[1].begin(), SMGAL[1].end(), s);
					SM2 = ix - SMGAL[1].begin();

					state++;
				}
				break;
			case 2:
				{
					char ch1 = *p_ch++;
					char ch2 = (p_ch < p_ch_end) ? *p_ch : _END_MARK;

					SM3 = 0;

					if (ch1 != '_')
					{
						// TODO: 曽失 採歳聖 蓄亜背醤 敗
					}

					state++;
				}
				break;
			case 3:
				{
					// uint code = uint(p_ch[0] & 0x0F) << 12 | uint(p_ch[1] & 0x3F) << 6 | uint(p_ch[2] & 0x3F);
					// xxxx1111 xx111122 xx222222
					// 11111111 22222222

					uint code = 0xAC00 + 28 * 21 * SM1 + 28 * SM2 + SM3;

					result += (unsigned char)(0xE0 | ((code >> 12) & 0xF));
					result += (unsigned char)(0x80 | ((code >> 6) & 0x3F));
					result += (unsigned char)(0x80 | ((code >> 0) & 0x3F));

					state = 0;
				}
				break;
			default:
				assert(false);
			}
		}

		return result;
	}
}

////////////////////////////////////////////////////////////////////////////////
// implementation (internal structure)

struct _KoreanNamer
{
	static const int  _ORDER = 3;
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

	int _num_ngrams;
	std::map<std::string, Ngram> _ngram_map;

	_KoreanNamer()
		: _num_ngrams(0)
	{
	}

	static _KoreanNamer* create(const std::vector<std::string>& texts)
	{
		_KoreanNamer* p_inst = new _KoreanNamer;
		int num_ngrams = 0;

		auto p_text_ = texts.begin();
		for (; p_text_ != texts.end(); ++p_text_)
		{
			if (p_text_->size() < _ORDER)
				continue;

			std::string text = *p_text_ + _END_MARK;
			assert(text.size() > _ORDER);

			for (int j = 0; j < text.size() - _ORDER; j++)
			{
				std::string str = text.substr(j, _ORDER);

				auto p_ngram_map = p_inst->_ngram_map.find(str);
				if (p_ngram_map == p_inst->_ngram_map.end())
				{
					p_inst->_ngram_map.insert(std::make_pair(str, Ngram(j == 0)));
					num_ngrams += (j == 0) ? 1 : 0;

					p_ngram_map = p_inst->_ngram_map.find(str);
					assert(p_ngram_map != p_inst->_ngram_map.end());
				}

				Ngram& ref_ngram = p_ngram_map->second;

				char ch = text[j + _ORDER];

				auto p_char_map = ref_ngram.id_map.find(ch);
				if (p_char_map != ref_ngram.id_map.end())
					p_char_map->second++;
				else
					ref_ngram.id_map[ch] = 1;

				ref_ngram.count++;
			}
		}

		p_inst->_num_ngrams = num_ngrams;

		return p_inst;
	}

	std::string _KoreanNamer::nameSomeone(int(*fn_getRandom)(int n)) const
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

		while (*result.rbegin() != _END_MARK)
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
		assert(*result.rbegin() == _END_MARK);

		return result.substr(0, result.size() - 1);
	}

};

////////////////////////////////////////////////////////////////////////////////
// implementation

KoreanNamer::KoreanNamer()
	: _p_impl(new _KoreanNamer)
{
}

KoreanNamer::~KoreanNamer()
{
	delete _p_impl;
}

KoreanNamer* KoreanNamer::create(const std::vector<std::string>& _texts)
{
	std::vector<std::string> texts;
	{
		std::for_each(_texts.begin(), _texts.end(), [&texts](const std::string k_str)
		{
			texts.push_back(_convKorToEng(k_str));
		});
	}

	KoreanNamer* p_inst = new KoreanNamer;
	p_inst->_p_impl = _KoreanNamer::create(texts);

	return p_inst;
}

std::string KoreanNamer::nameSomeone(int(*fn_getRandom)(int n)) const
{
	return (_p_impl != nullptr) ? _convEngToKor(_p_impl->nameSomeone(fn_getRandom)) : "";
}
