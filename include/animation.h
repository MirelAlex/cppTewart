#ifndef ANIMATION_H
#define ANIMATION_H

#include <cmath> // For cos and sin functions
#include "raylib.h" // For Vector3 and Quaternion
#include "raymath.h"
#include "stewart_platform.h" // Include StewartPlatform class
#include <iostream>
#include <stdexcept>
#include <vector>

enum class AnimationType {
    ROTATE,
    WOBBLE,
    TILT,
    LISSAJOUS,
    length
};

struct PathPoint {
    Vector3 position;
};

// class CircularBuffer {
// public:
//     std::vector<PathPoint> buffer;
// private:
//     size_t capacity;
//     size_t head;

// public:
//     CircularBuffer(size_t size) : buffer(size), capacity(size), head(0) {}

//     void AddPathPoint(const Vector3& position, const Color& color) {
//         buffer[head] = { position, color };
//         head = (head + 1) % capacity; // Circular increment
//     }

//     void Clear() {
//         head = 0; // Reset head to start clearing from the beginning
//     }

//     size_t GetSize(){
//         return head;
//     }
// };

class CircularBuffer {
private:
    int head;
    int tail;
    int capacity;

public:
    std::vector<PathPoint> buffer;
    Color color;
    // Constructor to intialize circular buffer's data
    // members
    CircularBuffer(int capacity)
    {
        this->color = RED;
        this->capacity = capacity;
        this->head = 0;
        this->tail = 0;
        buffer.resize(capacity);
    }

    // Function to add an element to the buffer
    void push_back(Vector3 element)
    {
        buffer[head].position = element;
        head = (head + 1) % capacity;
        if (head == tail) {
            tail = (tail + 1) % capacity;
        }
    }

    PathPoint& get_last_point() {
        if (empty()) {
            throw std::runtime_error("Buffer is empty");
        }
        return buffer[(head - 1 + capacity) % capacity];
    }

    // Function to remove an element from the buffer
    void pop_front()
    {
        if (empty()) {
            throw std::out_of_range("Buffer is empty");
        }
        tail = (tail + 1) % capacity;
    }

    // Function to check if the buffer is empty
    bool empty() const { return head == tail; }

    // Function to check if the buffer is full
    bool full() const
    {
        return (head + 1) % capacity == tail;
    }

    // Function to get the size of the buffer
    int size() const
    {
        if (head >= tail) {
            return head - tail;
        }
        return capacity - (tail - head);
    }

};

class Animation {
public:
    Animation(StewartPlatform* platform);
    void runAnimation();
    void drawPath();

    // float getPct() const;
    // void setPct(float pct);

    // float getDtFactor() const;
    void setDtFactor(float dtFactor);

    // AnimationType getAnimationTypeActive() const;
    void setAnimationTypeActive(AnimationType animationTypeActive);
    void setAnimationDuration(float duration);

    // bool isAnimationTypeEditMode() const;
    // void setAnimationTypeEditMode(bool animationTypeEditMode);

private:
    StewartPlatform* platform; // Pointer to the associated StewartPlatform

    Vector3 translation;
    Quaternion orientation;

    float pct;
    float dtFactor;
    float duration;
    AnimationType animationTypeActive;
    bool animationShouldCycle;
    bool animationTypeEditMode;

    CircularBuffer pathBuffer; // Adjust capacity as needed
    void addPathPoint(const Vector3& position);
    void clearPath();

    void applyTransformationToPlatform(Vector3 T, Quaternion O);
    void incrAnimationTime();

};

#endif // ANIMATION_H
