#ifndef __BBT_ASSERT_H__
#define __BBT_ASSERT_H__ 1

#ifdef DEBUG
#define BBT_DEBUGBREAK __debugbreak
void internarAssertImplemmentation(const char* expresion, const char* desc, const char* file, const char* function, int line);
#define BBT_ASSERT(_expression, _msg) \
	if(!(_expression))\
	{\
		internarAssertImplemmentation(#_expression, _msg, __FILE__, __FUNCTION__, __LINE__);\
		BBT_DEBUGBREAK();\
	}
#else
#define BBT_DEBUGBREAK
#define BBT_ASSERT(_expression, _msg)
#endif



#endif
