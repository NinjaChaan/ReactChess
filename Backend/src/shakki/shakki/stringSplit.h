#pragma once

typedef void(*split_fn)(const char*, size_t, void*);

void split(const char* str, char sep, split_fn fun, void* data) {
	unsigned int start = 0, stop;
	for (stop = 0; str[stop]; stop++) {
		if (str[stop] == sep) {
			fun(str + start, stop - start, data);
			start = stop + 1;
		}
	}
	fun(str + start, stop - start, data);
}

template <class Container>
void add_to_container(const char* str, size_t len, void* data) {
	Container* cont = static_cast<Container*>(data);
	cont->push_back(std::string(str, len));
}

template <class Container>
void stringSplit(const std::string& str, Container& cont, char delim = ' ') {
	split(str.c_str(), delim, static_cast<split_fn>(add_to_container<Container>), &cont);
}
