#pragma once

#include "nbs/base/Logger.hpp"

#include <cassert>
#include <cstddef>
#include <fstream>

namespace nbs {

    class ISerializable 
    {
    public:
        virtual void onSerialize(class Serializer*) = 0;
        virtual void onDeserialize(class Serializer*) = 0;
    };

    class Serializer
    {
    public:
        virtual void write(void* ptr, size_t size) = 0;
        virtual size_t read(void* ptr, size_t size) = 0;
        virtual void serPos(size_t) = 0;
        virtual size_t getPos() = 0;
        virtual void close() = 0;

    public:
        void writeString(const char*);
        void writeString(const std::string&);
        void writeI8(i8);
        void writeU8(u8);
        void writeI16(i16);
        void writeU16(u16);
        void writeI32(i32);
        void writeU32(u32);
        void writeI64(i64);
        void writeU64(u64);
        void writeBool(bool);
        void write(ISerializable*);

        void readString(std::string&);
        void readString(char*);
        void readI8(i8&);
        void readU8(u8&);
        void readI16(i16&);
        void readU16(u16&);
        void readI32(i32&);
        void readU32(u32&);
        void readI64(i64&);
        void readU64(u64&);
        void readBool(bool&);
        void read(ISerializable*);
    };


    class OSerializer : public Serializer
    {
    public:
        OSerializer(const char*);
        ~OSerializer();

        virtual void write(void* ptr, size_t size) override;
        virtual size_t read(void* ptr, size_t size) override {
            LogError("Current stream is not input serializer");
            return 0;
        }
        virtual void serPos(size_t pos) override {
            m_stream.seekp(pos);
        }
        virtual size_t getPos() override {
            return m_stream.tellp();
        }
        virtual void close() override;

    private:
        std::ofstream m_stream;
    };

    
    class ISerializer : public Serializer
    {
    public:
        ISerializer(const char*);
        ~ISerializer();

        virtual void write(void* ptr, size_t size) override {
            LogError("Current stream is not output serializer");
            assert(false);
        }
        virtual size_t read(void* ptr, size_t size) override;
        virtual void serPos(size_t pos) override {
            m_stream.seekg(pos, std::ios::beg);
        }
        virtual size_t getPos() override {
            return m_stream.tellg();
        }
        virtual void close() override;

    private:
        std::ifstream m_stream;
    };
}