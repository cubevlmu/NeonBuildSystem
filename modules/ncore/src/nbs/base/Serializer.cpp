#include "Serializer.hpp"

namespace nbs {

    OSerializer::OSerializer(const char* path)
        : m_stream {}
    {
        m_stream.open(path, std::ios::out | std::ios::binary);

        if (!m_stream.is_open()) {
            LogError("Failed to open file -> ", path);
            return;
        }
    }


    OSerializer::~OSerializer()
    {
        m_stream.flush();
        m_stream.close();
    }


    void OSerializer::write(void* ptr, size_t size)
    {
        m_stream.write((char*)ptr, size);
    }


    void OSerializer::close()
    {
        m_stream.flush();
        m_stream.close();
    }


    ISerializer::ISerializer(const char* path)
        : m_stream {}
    {
        m_stream.open(path, std::ios::in | std::ios::binary);

        if (!m_stream.is_open()) {
            LogError("Failed to open file -> ", path);
            return;
        }
    }


    ISerializer::~ISerializer()
    {
        m_stream.close();
    }


    size_t ISerializer::read(void* ptr, size_t size)
    {
        m_stream.read((char*)ptr, size);
        return size;
    }


    void ISerializer::close()
    {
        m_stream.close();
    }


    ////////////////////////////////////////////////////////////////////////
    //// write & read utils
    ////////////////////////////////////////////////////////////////////////
    void Serializer::writeString(const char* val)
    {
        writeU32(strlen(val));
        write((void*)val, sizeof(char) * strlen(val));
    }


    void Serializer::writeString(const std::string& val)
    {
        writeU32(val.length());
        write((void*)val.c_str(), sizeof(char) * val.length());
    }


    void Serializer::writeI8(i8 val)
    {
        write((void*)&val, sizeof(i8));
    }


    void Serializer::writeU8(u8 val)
    {
        write((void*)&val, sizeof(u8));
    }


    void Serializer::writeI16(i16 val)
    {
        write((void*)&val, sizeof(i16));
    }


    void Serializer::writeU16(u16 val)
    {
        write((void*)&val, sizeof(u16));
    }


    void Serializer::writeI32(i32 val)
    {
        write((void*)&val, sizeof(i32));
    }


    void Serializer::writeU32(u32 val)
    {
        write((void*)&val, sizeof(u32));
    }

    
    void Serializer::writeI64(i64 val)
    {
        write((void*)&val, sizeof(i64));
    }


    void Serializer::writeU64(u64 val)
    {
        write((void*)&val, sizeof(u64));
    }


    void Serializer::writeBool(bool val)
    {
        write((void*)&val, sizeof(bool));
    }


    void Serializer::write(ISerializable* val)
    {
        val->onSerialize(this);
    }


    void Serializer::readString(std::string& v)
    {
        u32 size {};
        readU32(size);
        v.resize(size);
        read(v.data(), size * sizeof(char));
    }


    void Serializer::readString(char* v)
    {
        u32 size {};
        readU32(size);
        read(v, size * sizeof(char));
    }


    void Serializer::readI8(i8& v)
    {
        read((void*)&v, sizeof(i8));
    }


    void Serializer::readU8(u8& v)
    {
        read((void*)&v, sizeof(u8));
    }


    void Serializer::readI16(i16& v)
    {
        read((void*)&v, sizeof(i16));
    }


    void Serializer::readU16(u16& v)
    {
        read((void*)&v, sizeof(u16));
    }


    void Serializer::readI32(i32& v)
    {
        read((void*)&v, sizeof(i32));
    }


    void Serializer::readU32(u32& v)
    {
        read((void*)&v, sizeof(u32));
    }


    void Serializer::readI64(i64& v)
    {
        read((void*)&v, sizeof(i64));
    }


    void Serializer::readU64(u64& v)
    {
        read((void*)&v, sizeof(u64));
    }


    void Serializer::readBool(bool& v)
    {
        read((void*)&v, sizeof(bool));
    }


    void Serializer::read(ISerializable* v)
    {
        v->onDeserialize(this);
    }

}