#pragma once
//---------------------------------------------------------------------------
// Date: 14/05/2013
// Author: Boris Mazic
// Abstract:
//		An abstraction over the array of characters with methods provided
//		to ease extraction of tokens from the array.
//		Intended to be used for parsing a command line without the need to 
//		convert parts of the command line into C (nul-terminated) strings.
//		Chunks are parts of the character array represented by starting
//		and ending indices.
//
/*
	Buffer input;
	...
	loop {
		input += ch;
		...
		Buffer::Chunk cmd = input.next_token(0, WHITESPACE);
		if(0 == cmd.cmp("power_down")) {
			Buffer::Chunk p = cmd.next_token(WHITESPACE);
			if(0 == p.cmp("true") || 0 == p.cmp("1")) {
				...
			}
			...
		}
		...
		input.truncate();
	}
*/
//---------------------------------------------------------------------------
/*
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

class Buffer {
public:
	static int const BUFFER_SIZE = 128;
	class Chunk { 
	public:
		int a, b;
	private:
		Buffer const *_buffer;
		
	public:
		explicit Chunk(Buffer const &buf) : a(0), b(0), _buffer(&buf) {
		}
		Chunk& operator=(Chunk const &rhs) {
			a = rhs.a;
			b = rhs.b;
			_buffer = rhs._buffer;
			return *this;
		}
		Chunk(int i, int j, Buffer const &buf) : a(i), b(j), _buffer(&buf) {
		}
		
		int len() const {
			return b-a;
		}
		
		int cmp(char const *s) const {
			int i = a;
			for(; i < b && *s; ++i, ++s) {
				if(_buffer->_data[i] == *s) continue;
				return _buffer->_data[i] < *s ? -1 : 1;
			}
			if(!(i < b) && !*s) {
				return 0;
			} else {
				return *s ? -1 : 1;
			}
		}
		
		// return the next token delimited by the characters from the supplied set
		Chunk next_token(char const *set) const {
			return _buffer->next_token(b,set);
		}
		
		void c_str(char *s) const {
			memcpy(s, &_buffer->_data[a], b-a);
			s[b-a] = 0;
		}
		
		// set intersection operator
		Chunk operator&(Chunk const &rhs) const {
			Chunk const &lhs = *this;
			bool overlap = (lhs.a >= rhs.a && lhs.a < rhs.b) || (rhs.a >= lhs.a && rhs.a < lhs.b);
			if(overlap) {
				return Chunk(max(lhs.a,rhs.a), min(lhs.b,rhs.b), *_buffer);
			}
			return Chunk(0,0,*_buffer);
		}

		// (simplified) set difference operator
		Chunk operator-(Chunk const &rhs) const {
			Chunk const &lhs = *this;
			Chunk r = lhs;
			if(lhs.a <= rhs.a) {
				r.b = min(lhs.b, rhs.a);
			} else {
				r.a = max(lhs.a, rhs.b);
			}
			if(r.b < r.a) {
				r = Chunk(0,0,*_buffer);
			}
			return r;
		}
	};
	
	Buffer() : _length(0) {
	}
	
	char& operator[](int i) {
		return _data[i];
	}
	
	const char& operator[](int i) const {
		return _data[i];
	}
	
	Buffer& operator+=(char ch) {
		_data[_length++] = ch;
		return *this;
	}
	
	void truncate() {
		_length = 0;
	}
	
	static
	bool in(char c, char const *set) {
		for(; *set; ++set) {
			if(*set == c) return true;
		}
		return false;
	}
	
	Chunk all() const {
		return Chunk(0,_length,*this);
	}
	
	// return a span of characters from the beginning of the chunk that belong to the supplied set.
	Chunk span(Chunk const &c, char const *set, bool complement = false) const {
		char const *p = _data + c.a;
		char const *end = _data + c.b;
		int b = c.a;
		for(; p < end ;++b, ++p) {
			bool predicate = !in(*p , set);
			if(complement) predicate = !predicate; 
			if(predicate) return Chunk(c.a, b, *this);
		}
		return Chunk(c.a, b, *this);
	}
	Chunk span(int i, char const *set, bool complement = false) const {
		return span(Chunk(i, _length, *this), set, complement);
	}
	
	// return a span of characters from the beginning of the chunk that DO NOT belong to the supplied set.
	Chunk span_c(Chunk const &c, char const *set) const {
		return span(c,set,true);
	}
	Chunk span_c(int i, char const *set) const {
		return span(Chunk(i, _length, *this), set, true);
	}
	
	// return a span of characters from the end of the chunk that belong to the supplied set.
	Chunk rspan(Chunk const &c, char const *set, bool complement = false) const {
		char const *p = _data + c.b - 1;
		char const *end = _data + c.a - 1;
		int a = c.b;
		for(; p > end ;--a, --p) {
			bool predicate = !in(*p , set);
			if(complement) predicate = !predicate; 
			if(predicate) return Chunk(a, c.b, *this);
		}
		return Chunk(a, c.b, *this);
	}
	Chunk rspan(int i, char const *set, bool complement = false) const {
		return span(Chunk(i, _length, *this), set, complement);
	}
	
	// return a span of characters from the end of the chunk that DO NOT belong to the supplied set.
	Chunk rspan_c(Chunk const &c, char const *set) const {
		return rspan(c,set,true);
	}
	Chunk rspan_c(int i, char const *set) const {
		return rspan(Chunk(i, _length, *this), set, true);
	}
	
	// trim the chunk, both left and right.
	Chunk trim(Chunk const &c, char const *set) const {
		Chunk left = span(c,set);
		Chunk right = rspan(c,set);
		return Chunk(left.b, right.a, *this);
	}
	
	// return the next token delimited by the characters from the supplied set
	Chunk next_token(int i, char const *set) const {
		Chunk left = span(Chunk(i,_length, *this),set);
		return span_c(left.b,set);
	}
	
private:
	char _data[BUFFER_SIZE];
	int _length;
};

