#ifndef ENCRYPTED_STRING_H
#define ENCRYPTED_STRING_H

#include <cstddef>
#include <iostream>

#include "encryptor.h"
#include "iterator.h"

/**
 * @file encrypted_string.h
 * @brief Titkosított string osztály deklarációja.
 */

/**
 * @class EncryptedString
 * @brief Titkosított formában tárolt szöveg.
 *
 * A karaktereket titkosítva tárolja, de a felhasználó
 * számára dekódolt formában biztosítja az elérést.
 */
class EncryptedString
{
private:
    char *data = nullptr; /**< Titkosított karakterek */
    size_t length = 0;    /**< Aktuális hossz */
    size_t capacity = 0;  /**< Kapacitás */
    const Encryptor &enc; /**< Titkosítási algoritmus */

    void reserve(size_t newCap)
    {
        if (newCap <= 0)
        {
            newCap = 2;
        }

        capacity = newCap;

        char *newData = new char[capacity];

        for (size_t i = 0; i < capacity; i++)
        {
            newData[i] = 0;
        }

        for (size_t i = 0; i < length; i++)
        {
            newData[i] = data[i];
        }

        delete[] data;

        data = newData;
    }

public:
    /**
     * @brief Konstruktor.
     */
    explicit EncryptedString(const Encryptor &enc) : enc(enc)
    {
        reserve(0);
        clear();
    }

    /**
     * @brief Konstruktor C-stringből.
     */
    EncryptedString(const char *text, const Encryptor &enc) : enc(enc)
    {
        const char *ch = text;
        while (*ch != '\0')
        {
            this->push_back(*ch);
            ch++;
        }
    }

    /**
     * @brief Másoló konstruktor.
     */
    EncryptedString(const EncryptedString &other) : enc(other.enc)
    {
        reserve(other.capacity);

        for (size_t i = 0; i < other.length; i++)
        {
            data[i] = other[i];
        }

        length = other.length;
    }

    /**
     * @brief Destruktor.
     */
    ~EncryptedString()
    {
        clear();
        delete[] data;
    }

    /**
     * @brief Értékadás operátor.
     */
    EncryptedString &operator=(const EncryptedString &other)
    {
        if (*this == other)
        {
            return *this;
        }

        delete[] data;

        length = other.length;
        capacity = other.capacity;
        data = new char[capacity];

        for (size_t i = 0; i < length; i++)
        {
            data[i] = other[i];
        }

        return *this;
    };

    /**
     * @brief Indexelés.
     */
    char &operator[](size_t index) const
    {
        if (index < 0 || index >= length)
        {
            throw "Index out of range";
        }

        return data[index];
    }

    /**
     * @brief Összefűzés.
     * mindig a bal érték szerinti encryptort használja
     */
    EncryptedString operator+(const EncryptedString &rhs)
    {
        EncryptedString newenc(*this);

        for (Iterator i = rhs.begin(); i != rhs.end(); ++i)
        {
            newenc.push_back(*i);
        }

        return newenc;
    }

    /**
     * @brief Összefűzés és értékadás.
     * mindig a bal érték szerinti encryptort használja
     */
    EncryptedString &operator+=(const EncryptedString &rhs)
    {
        if (rhs.size() == 0)
        {
            return *this;
        }

        for (Iterator it = rhs.begin(); it != rhs.end(); ++it)
        {
            push_back(*it);
        }

        return *this;
    }

    bool equals(const EncryptedString &rhs) const
    {
        if (length != rhs.length || capacity != rhs.capacity || enc != rhs.enc)
        {
            return false;
        }

        for (size_t i = 0; i < length; i++)
        {
            if (this->data[i] != rhs.data[i])
            {
                return false;
            }
        }

        return true;
    }

    /**
     * @brief Összehasonlítás.
     */
    bool operator==(const EncryptedString &rhs) const
    {
        return equals(rhs);
    }

    bool operator!=(const EncryptedString &rhs) const
    {
        return !equals(rhs);
    };

    /**
     * @brief Karakter hozzáadása.
     */
    void push_back(char c)
    {
        if (length >= capacity)
        {
            reserve(capacity * 2);
        }
        data[length] = enc.encode(c);

        length++;
    }

    /**
     * @brief Méret lekérdezése.
     */
    size_t size() const
    {
        return length;
    }

    /**
     * @brief String törlése.
     */
    void clear()
    {
        for (size_t i = 0; i < capacity; i++)
        {
            data[i] = 0;
        }
    }

    char decode_at(size_t index) const
    {
        return enc.decode(data[index]);
    }

    /**
     * @brief Iterátor kezdete.
     */
    Iterator begin() const
    {
        return Iterator(*this, 0);
    }

    /**
     * @brief Iterátor vége.
     */
    Iterator end() const
    {
        return Iterator(*this, length);
    }

    //TODO implement this
    std::ostream &operator<<(ostream& out, const EncryptedString& str) ;
};

#endif
