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
    char *data = nullptr; /**< Titkosított karakterek tömbje. */
    size_t length = 0;    /**< A titkosított string aktuális hossza (karakterek száma). */
    size_t capacity = 0;  /**< A lefoglalt memória kapacitása (karakterek száma). */
    const Encryptor &enc; /**< A titkosításhoz használt Encryptor objektum referenciája. */

    /**
     * @brief Növeli vagy csökkenti a string kapacitását.
     *
     * Új memóriaterületet foglal le a megadott kapacitással,
     * átmásolja a meglévő adatokat, majd felszabadítja a régi memóriát.
     * Ha `newCap` <= 0, akkor a kapacitás 2 lesz.
     *
     * @param newCap Az új kapacitás, amit be kell állítani.
     */
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
     * @brief Alapértelmezett konstruktor.
     *
     * Létrehoz egy üres titkosított stringet a megadott titkosítóval.
     *
     * @param enc A titkosításhoz használt Encryptor objektum referenciája.
     */
    explicit EncryptedString(const Encryptor &enc) : enc(enc)
    {
        reserve(0);
        clear();
    }

    /**
     * @brief Konstruktor C-stílusú stringből.
     *
     * Létrehoz egy titkosított stringet egy null-terminált C-stílusú string tartalmából.
     *
     * @param text A forrás C-stílusú string.
     * @param enc A titkosításhoz használt Encryptor objektum referenciája.
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
     *
     * Létrehoz egy új titkosított stringet egy másik EncryptedString objektum másolásával.
     *
     * @param other A másolandó EncryptedString objektum.
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
     *
     * Felszabadítja a string által lefoglalt memóriát.
     */
    ~EncryptedString()
    {
        clear();
        delete[] data;
    }

    /**
     * @brief Értékadás operátor.
     *
     * Értéket ad egy EncryptedString objektumnak egy másik EncryptedString objektumból.
     *
     * @param other A jobb oldali EncryptedString objektum.
     * @return Referencia a bal oldali objektumra (*this).
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
     * @brief Indexelő operátor (konstans).
     *
     * Hozzáférést biztosít a titkosított string egy adott indexű karakteréhez.
     * A visszaadott karakter titkosított formában van.
     *
     * @param index A hozzáférni kívánt karakter indexe.
     * @return Referencia a titkosított karakterre.
     * @throw const char* Ha az index kívül esik a string határain.
     */
    char &operator[](size_t index) const
    {
        if (index < 0 || index >= length) // Megjegyzés: size_t típus esetén az index < 0 mindig hamis.
        {
            throw "Index out of range";
        }

        return data[index];
    }

    /**
     * @brief Összefűzés operátor.
     *
     * Két EncryptedString objektumot fűz össze egy új objektumba.
     * Mindig a bal oldali operandus titkosítóját használja.
     *
     * @param rhs A jobb oldali EncryptedString objektum.
     * @return Egy új EncryptedString objektum, amely a két string összefűzött tartalmát tartalmazza.
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
     * @brief Összefűzés és értékadás operátor.
     *
     * Hozzáfüz egy EncryptedString objektumot a jelenlegi objektumhoz.
     * Mindig a bal oldali operandus titkosítóját használja.
     *
     * @param rhs A jobb oldali EncryptedString objektum.
     * @return Referencia a bal oldali objektumra (*this).
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

    /**
     * @brief Két EncryptedString objektum tartalmának összehasonlítása.
     *
     * Összehasonlítja a jelenlegi objektumot egy másik EncryptedString objektummal.
     * Ellenőrzi a hosszt, kapacitást, titkosítót és a titkosított adatokat.
     *
     * @param rhs A jobb oldali EncryptedString objektum.
     * @return Igaz, ha a két string egyenlő, egyébként hamis.
     */
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
     * @brief Egyenlőség operátor.
     *
     * Ellenőrzi, hogy két EncryptedString objektum egyenlő-e.
     *
     * @param rhs A jobb oldali EncryptedString objektum.
     * @return Igaz, ha a két string egyenlő, egyébként hamis.
     */
    bool operator==(const EncryptedString &rhs) const
    {
        return equals(rhs);
    }

    /**
     * @brief Nem egyenlőség operátor.
     *
     * Ellenőrzi, hogy két EncryptedString objektum nem egyenlő-e.
     *
     * @param rhs A jobb oldali EncryptedString objektum.
     * @return Igaz, ha a két string nem egyenlő, egyébként hamis.
     */
    bool operator!=(const EncryptedString &rhs) const
    {
        return !equals(rhs);
    };

    /**
     * @brief Karakter hozzáadása a string végéhez.
     *
     * Hozzáad egy karaktert a string végéhez, titkosítva azt a belső Encryptor segítségével.
     * Szükség esetén növeli a kapacitást.
     *
     * @param c A hozzáadandó karakter.
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
     * @brief A string aktuális méretének lekérdezése.
     *
     * @return A stringben tárolt karakterek száma.
     */
    size_t size() const
    {
        return length;
    }

    /**
     * @brief A string tartalmának törlése.
     *
     * A string összes karakterét nullára állítja, de a lefoglalt memóriát megtartja.
     */
    void clear()
    {
        for (size_t i = 0; i < capacity; i++)
        {
            data[i] = 0;
        }
    }

    /**
     * @brief Egy adott indexű karakter dekódolása.
     *
     * Visszaadja a megadott indexen lévő titkosított karakter dekódolt változatát.
     *
     * @param index A dekódolandó karakter indexe.
     * @return A dekódolt karakter.
     */
    char decode_at(size_t index) const
    {
        return enc.decode(data[index]);
    }

    /**
     * @brief Iterátor a string elejére.
     *
     * Létrehoz és visszaad egy iterátort, amely a string első elemére mutat.
     *
     * @return Egy Iterator objektum, amely a string elejére mutat.
     */
    Iterator begin() const
    {
        return Iterator(*this, 0);
    }

    /**
     * @brief Iterátor a string végére.
     *
     * Létrehoz és visszaad egy iterátort, amely a string utolsó eleme utáni pozícióra mutat.
     *
     * @return Egy Iterator objektum, amely a string végére mutat.
     */
    Iterator end() const
    {
        return Iterator(*this, length);
    }

    //TODO implement this
    /**
     * @brief Kimeneti stream operátor.
     *
     * Kiírja az EncryptedString tartalmát egy kimeneti streamre dekódolt formában.
     *
     * @param out A kimeneti stream.
     * @param str A kiírandó EncryptedString objektum.
     * @return A kimeneti stream referenciája.
     */
    std::ostream &operator<<(ostream& out, const EncryptedString& str) ;
};

#endif
