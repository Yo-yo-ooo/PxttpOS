#pragma once

#include <libm/heap/heap.h>

namespace std{

template<typename T> 
class vector{
public:
    typedef T value_type;
    typedef T* iterator;
private:
    value_type* _data;
    size_t _size;
    size_t _capacity;

public:
    vector():_data(NULL),_size(0),_capacity(0){}
    ~vector(){
        ///delete [] _data;
        _Free((void*)_data);
        _data = NULL;
        _size = 0;
        _capacity = 0;
    }
    vector(const vector& vec){
        _size = vec._size;
        _capacity = vec._capacity;
        //_data = new value_type[_capacity];
        _data = (value_type)_Malloc1(_capacity);
        for(int i=0;i<_size;++i){
            _data[i] = vec._data[i];
        }
    }
    vector& operator=(const vector& vec){
        if(this == &vec) return *this;
        value_type* temp = (value_type)_Malloc1(vec._capacity);
        for(int i=0;i<vec._size;++i){
            temp[i] = vec._data[i];
        }
        //delete [] _data;
        _Free((void*)_data);
        _data = temp;
        _size = vec._size;
        _capacity = vec._capacity;
        return *this;
    }
    void push_back(value_type val){
        if(0 == _capacity){
           _capacity = 1;
           _data = new value_type[1];
        }else if(_size+1 > _capacity){
           _capacity *= 2;
           value_type* temp = new value_type[_capacity];
           for(int i=0;i<_size;++i){
             temp[i] = _data[i];
           }
           //delete [] _data;
           _Free((void*)_data);
           _data = temp;
        }
        _data[_size] = val;
        ++_size;
    }
    void pop_back(){ --_size;}
    size_t size()const{ return _size;}
    size_t capacity()const { return _capacity;}
    bool empty(){ return _size == 0;}
    value_type& operator[](size_t index){
        return _data[index];
    }
    bool operator==(const vector& vec)const{
        if(_size != vec._size) return false;
        for(int i=0;i<_size;++i){
            if(_data[i] != vec._data[i]) return false;
        }
        return true;
    }
    value_type front()const{ return _data[0];}
    value_type back() const{ return _data[_size-1];}
    
    void insert(iterator it,value_type val){
        int index = it - _data;
        if(0 == _capacity){
            _capacity = 1;
            _data = new value_type[1];
            _data[0] = val;
        }else if(_size+1 > _capacity){
            _capacity *= 2;
            value_type* temp = new value_type[_capacity];
            for(int i=0;i<index;++i){
                temp[i] = _data[i];
            }
            temp[index] = val;
            for(int i=index;i<_size;++i){
                temp[i+1] = _data[i];
            }
            //delete [] _data;
            _Free((void*)_data);
            _data = temp;
        } else {
            for(int i=_size-1;i>=index;--i){
                _data[i+1] = _data[i];
            }
            _data[index] = val;
        }
        ++_size; 
    
    }
    void erase(iterator it){
        size_t index = it - _data;
        for(int i=index;i<_size-1;++i){
            _data[i] = _data[i+1];
        }
        --_size;
    }
    
    iterator begin(){ return _data; }
    iterator end(){ return _data + _size; }
};

}
