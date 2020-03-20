#pragma once
#include <iostream>
#include <cassert>

template<typename T> class has_typedef_size_t {
private:
	typedef char Yes;
	typedef Yes No[2];

	template<typename U, U> struct really_has;

	template<typename C> static Yes& Test(really_has <size_t(C::*)() const, &C::size>*);

	template<typename> static No& Test(...);
public:
	static bool const value = sizeof(Test<T>(0)) == sizeof(Yes);
};

template<typename T> class List;
template<typename T> class Head;

template<typename T> 
class ListNode {
	friend class Head<T>;
	friend class List<T>;
public:
	T data;
	ListNode* next = nullptr;
	ListNode* prev = nullptr;
	ListNode* rand = nullptr;
};

template<typename T> class Head {
public:
	ListNode<T>* node;
	Head() : node(nullptr) {}
	~Head() {}
	void operator=(ListNode<T>* p_n) { node = p_n; }
	T& operator* () {
		assert(node != nullptr);
		return node->data;
	}
	void operator++() {
		assert(node != nullptr);
		node = node->next;
	}
	void operator++(int) {
		assert(node != nullptr);
		node = node->next;
	}
	void operator--() {
		assert(node != nullptr);
		node = node->prev;
	}
	void operator--(int) {
		assert(node != nullptr);
		node = node->prev;
	}
	bool operator !=(ListNode<T>* p_n) { return (node != p_n); }
	bool operator ==(ListNode<T>* p_n) { return(node == p_n); }
};

template<typename T> class List {
public:
	int size;
	ListNode<T>* root;
	ListNode<T>* lastNode;
	List() : size(0), root(0), lastNode(0) {}
	~List() {
		while (root != nullptr) {
			Pop();
		}
	}
	ListNode<T>* Begin() {
		assert(root != nullptr);
		return root;
	}
	ListNode<T>* End() {
		return nullptr;
	}
	void Push(T p_newData) {
		ListNode<T>* node = new ListNode<T>;
		assert(node != nullptr);
		node->data = p_newData;
		if (root != nullptr) {
			root->prev = node;
		}
		node->next = root;
		root = node;
		if (root->next == nullptr) {
			lastNode = node;
		}
		size++;
	}
	void Pop() {
		assert(root != nullptr);
		if (root->next == nullptr) {
			delete root;
			root = nullptr;
		}
		else {
			ListNode<T>* prevNode = root;
			while (prevNode->next != nullptr && prevNode->next != lastNode) {
				prevNode = prevNode->next;
			}
			delete lastNode;
			prevNode->next = nullptr;
			lastNode = prevNode;
		}
		size = (size == 0 ? size : size - 1);
	}
	void Pop_front() {
		assert(lastNode != nullptr);
		if (lastNode->prev == nullptr) {
			delete lastNode;
			lastNode = nullptr;
		}
		else {
			ListNode<T>* nextNode = lastNode;
			while (nextNode->prev != nullptr && nextNode->prev != root) {
				nextNode = nextNode->prev;
			}
			delete root;
			nextNode->prev = nullptr;
			root = nextNode;
		}
		size = (size == 0 ? size : size - 1);
	}
	void Pop(T &p_tempdata) {
		assert(root != nullptr);
		if (root->next == nullptr) {
			p_tempdata = root->data;
			delete root;
			root = nullptr;
		}
		else {
			ListNode<T>* prevNode = root;
			while (prevNode->next != nullptr && prevNode->next != lastNode) {
				prevNode = prevNode->next;
			}
			p_tempdata = lastNode->data;
			delete lastNode;
			prevNode->next = nullptr;
			lastNode = prevNode;
		}
		size = (size == 0 ? size : size - 1);
	}
	int GetSize() { return size; };
	template<typename T>
	typename std::enable_if<has_typedef_size_t<T>::value>::type
		toByte(T p_data, FILE* p_file) {
		int size = p_data.size();
		std::fwrite(reinterpret_cast<char*>(&size), sizeof(size), 1, p_file);
		std::fwrite(p_data.data(), sizeof(char), size, p_file);
	}

	template<typename T>
	typename std::enable_if<!has_typedef_size_t<T>::value>::type
		toByte(T p_data, FILE* p_file) {
		std::fwrite(reinterpret_cast<char*>(&p_data), sizeof(p_data), 1, p_file);
	}
	template<typename T>
	typename std::enable_if<has_typedef_size_t<T>::value>::type
	fromByte(T p_data,FILE* p_file) {
		int size=0;
		std::fread(reinterpret_cast<char*>(&size), sizeof(int), 1, p_file);
		p_data.resize(size);
 		std::fread(&p_data[0], sizeof(char), size, p_file);
		Push(p_data);
	}
	template<typename T>
	typename std::enable_if<!has_typedef_size_t<T>::value>::type
	fromByte(T p_data, FILE* p_file) {
		std::fread(reinterpret_cast<char*>(&p_data), sizeof(p_data), 1, p_file);
		Push(p_data);
	}
	void Serialize(FILE* p_file) {
		ListNode<T>* node = lastNode;
		while (node != nullptr) {
			toByte(node->data, p_file);
			node=node->prev;
			std::fflush(p_file);
		}
	}

	void Unserialize(T p_data, FILE* p_file) {
		while (!std::feof(p_file)) {
			fromByte(p_data, p_file);
		}
		Pop_front(); //To remove the last empty element of EOF
	}
};