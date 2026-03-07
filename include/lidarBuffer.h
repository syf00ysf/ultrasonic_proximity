#ifndef LIDARBUFFER_H
#define LIDARBUFFER_H

#include <cstddef>
#include <optional>
#include <vector>

template <typename T>

class LidarRingBuffer{
    private:
    size_t max_size;
    size_t head;
    size_t tail;
    size_t count;
    T* buffer;

    public:
    //constructor
    LidarRingBuffer(const size_t max_size): max_size { max_size }{
        head = 0;
        tail = 0;
        count = 0;
        buffer = new T[max_size];
    }
    //Destructor
    ~LidarRingBuffer(){
        delete [] buffer;
    }

    //copy constructors
    LidarRingBuffer(const LidarRingBuffer& other)
        :max_size { other.max_size },
         head { other.head },
         tail { other.tail },
         count { other.count },
         buffer { new T[max_size] } {
            for(size_t i = 0; i < max_size; i++){
                buffer[i] = other.buffer[i];
        }
        
    }

    LidarRingBuffer& operator=(const LidarRingBuffer& other){
        if (this == &other) return *this;
        delete [] buffer;
        max_size = other.max_size;
        buffer = new T[max_size];
        for(size_t i = 0; i < max_size; i++){
            buffer[i] = other.buffer[i];
        }
        head = other.head;
        tail = other.tail;
        count = other.count;
        return *this;
    }
    //move constructors
    LidarRingBuffer(LidarRingBuffer&& other)noexcept
        :max_size { other.max_size },
         head {other.head },
         tail { other.tail },
         count { other.count },
         buffer { other.buffer }{
        other.max_size = 0;
        other.head = 0;
        other.tail = 0;
        other.count = 0;
        other.buffer = nullptr;
    }
    LidarRingBuffer& operator=(LidarRingBuffer&& other)noexcept{
        if (this == &other) return *this;
        delete [] buffer;
        max_size = other.max_size;
        head = other.head;
        tail = other.tail;
        count = other.count;
        buffer = other.buffer;
        other.max_size = 0;
        other.head = 0;
        other.tail = 0;
        other.count = 0;
        other.buffer = nullptr;
        return *this;
    }

    void push_to_ring(T&& item){
        buffer[head] = std::move(item);
        head = (head + 1) % max_size;
        if (count == max_size){
            tail = (tail + 1) % max_size;     
        }
        else{
            count++;
        }    
    }

    void push_to_ring(const T& item){
        buffer[head] = item;
        head = (head + 1) % max_size;
        if (count == max_size){
            tail = (tail + 1) % max_size;
        }
        else{
            count++;
        }
    }

    std::optional <T> pop_from_ring(){
        if (count == 0) return std::nullopt;
        T item = buffer[tail];
        tail = (tail + 1) % max_size;
        count--;
        return item;
    }

    std::vector <T> get_recent_readings(size_t num_items){
        if(num_items > count){
            num_items = count;
        }
        std::vector <T> recent_readings(num_items);

        size_t current_head = (head + max_size - 1) % max_size;
        for(size_t i = 0; i < num_items; i++){
            recent_readings[i] = buffer[current_head];
            current_head = (current_head + max_size - 1) % max_size;
        }
        return recent_readings;
        
    }
    

};

#endif