/*
	Copyright (c) 2010 - 2011 conleon1988 (conleon1988@gmail.com)

	Permission is hereby granted, free of charge, to any person
	obtaining a copy of this software and associated documentation
	files (the "Software"), to deal in the Software without
	restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef _NEON64_PlugScriptCompiler_
#define _NEON64_PlugScriptCompiler_
#include <iostream>

class CNEON64_ScriptCompiler
{
	private:
		std::string m_Code;

	public:
	CNEON64_ScriptCompiler()
	{
		clear();
	}

	~CNEON64_ScriptCompiler()
	{
		clear();
	}

	inline void clear()
	{
		m_Code = "";
	}

	inline void reserve(const int size)
	{
		clear();
		m_Code.reserve(size);
	}

	inline void compile(const char* identifier,const char* value)
	{
		m_Code += "$";
		m_Code += std::string(identifier);
		m_Code += "=";
		m_Code += std::string(value);
		m_Code += ";\n";
	}

	inline const char* getCodePtr()
	{
		return m_Code.c_str();
	}

	inline const int getCodeLength()
	{
		return m_Code.length();
	}

};

#endif

