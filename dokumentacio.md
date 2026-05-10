# 14. Forráskód kommentálása és magyarázat generálása

**Készítette:** Szabó Márton

A feladatot önállóan készítettem el.

---

## A feladat rövid leírása

A választott feladat célja egy olyan AI-alapú eszköz készítése volt, amely képes forráskód automatikus kommentálására és magyarázat generálására. A projekt során egy egyszerű, parancssorból futtatható programot készítettem, amely C++ forrásfájlokat olvas be, majd LLM segítségével Doxygen-formátumú kommentekkel látja el azokat.

A projekt eredeti célja egy összetettebb, automatikusan működő fejlesztői segédeszköz létrehozása volt, például VS Code extension vagy fájlfigyelő rendszer formájában. A rendelkezésre álló idő és a megvalósíthatóság figyelembevételével végül egy egyszerűbb, de működőképes MVP készült el. Ez a verzió egy megadott C++ fájlt dolgoz fel, majd egy külön kimeneti fájlba menti a kommentekkel ellátott változatot.

A megoldás során fontos szempont volt, hogy a generált kommentek ne változtassák meg a program működését, hanem kizárólag a kód olvashatóságát és dokumentáltságát javítsák. Emiatt a promptban külön szabályokkal korlátoztam az LLM viselkedését: nem módosíthatja a program logikáját, nem nevezhet át változókat, és Doxygen-kompatibilis kommenteket kell készítenie.

---

## A megoldás rövid áttekintése

A program egy egyszerű CLI toolként működik.

Példa használat:

```bash
python autodoxy.py examples/calculator.cpp outputs/calculator_commented.cpp
```

## A program működési folyamata:

Beolvassa a megadott C++ forrásfájlt.
Ellenőrzi, hogy a bemeneti fájl létezik-e és támogatott kiterjesztésű-e.
A fájl tartalmát elküldi egy LLM API-nak.
Az LLM Doxygen-stílusú kommenteket generál a kódhoz.
A kommentelt kódot elmenti a megadott kimeneti fájlba.

A projekt célja nem egy teljes értékű ipari dokumentációgenerátor elkészítése volt, hanem annak bemutatása, hogyan lehet LLM-et használni forráskód automatikus dokumentálására prompt engineering technikák segítségével.

## Használt technológiák
- Python – a CLI tool megvalósításához
- Google Gemini API – az LLM-alapú kommentgeneráláshoz Doxygen-formátum – a C++ kommentek szabványos dokumentálásához
- ChatGPT – a projekt megtervezéséhez és a dokumentáció elkészítéséhez
- OpenAI Codex – a script megírásának támogatásához
- Ollama – lokális modellek kipróbálásához a projekt korai szakaszában
- OpenClaw – AI agent alapú megközelítés kipróbálásához és értékeléséhez

## Prompt engineering megközelítés

A projekt egyik legfontosabb része a megfelelő prompt megtervezése volt. A prompt célja az volt, hogy az LLM ne egyszerűen általános kommenteket írjon, hanem szabályozott, Doxygen-formátumú dokumentációt készítsen C++ kódhoz.

### A használt prompt főbb elemei:
```text
You are a senior C++ developer.

Add Doxygen-style comments to the following C++ code.

Rules:
- Do not change the program logic.
- Do not rename variables.
- Do not reformat unrelated code.
- Add /** ... */ Doxygen comments before classes and functions.
- Use @brief, @param, and @return where appropriate.
- Return only the full commented C++ code.

Code:
{SOURCE_CODE}
```
### A promptban szereplő fontosabb döntések:

- Szerep megadása: az LLM senior C++ fejlesztőként kapja a feladatot.
- Kimeneti formátum rögzítése: Doxygen-kommenteket kell használnia.
- Kódmódosítás tiltása: a modell nem változtathatja meg a program működését.
- Tömör, célzott szabályok: a prompt nem túl hosszú, de egyértelműen meghatározza az elvárt viselkedést.
- Teljes kód visszakérése: a program így egyszerűen el tudja menteni a kommentelt verziót.

### Példa bemenet
```cpp
int add(int a, int b) {
    return a + b;
}
```

### Példa kimenet
```cpp
/**
 * @brief Adds two integers.
 *
 * @param a First integer.
 * @param b Second integer.
 * @return The sum of the two integers.
 */
int add(int a, int b) {
    return a + b;
}
```

## Projektstruktúra

A projekt javasolt fájlstruktúrája:
```
autodoxy/
├── autodoxy.py
├── README.md
├── dokumentacio.md
├── examples/
    ├── encrypted_string.h  //Prog2 nagy HF egyik része
    ├── out.h               //kész kommentezett rész
    └── student.hpp

```
## Megvalósítás

A megvalósított program bemeneti paraméterként kap egy forrásfájlt és egy kimeneti fájlnevet. Ezután a forráskódot szövegként beolvassa, majd a prompttal együtt elküldi az LLM API-nak.

A válaszként kapott kommentelt kódot a program változtatás nélkül kiírja a megadott kimeneti fájlba. A megoldás egyszerűsége miatt nem használ C++ parser-t vagy AST-alapú elemzést, hanem az LLM-re bízza a teljes forrásfájl feldolgozását.

Ez a megközelítés gyorsan elkészíthető és jól demonstrálja az LLM-ek használatát, ugyanakkor vannak korlátai. Nagyobb fájloknál, bonyolultabb projekteknél vagy több fájlból álló kódbázisnál már érdemes lenne strukturáltabb megoldást használni, például függvényenkénti feldolgozást vagy C++ parser integrációt.

## Tesztelés

A programot több egyszerű C++ példán teszteltem:

egyszerű matematikai függvények
osztályokat tartalmazó header fájlok
rendező algoritmusokat tartalmazó példák

### A tesztelés során azt ellenőriztem, hogy:

- létrejön-e a kimeneti fájl,
- a kommentek Doxygen-formátumban készülnek-e el,
- a program logikája változatlan marad-e,
- az LLM használ-e @brief, @param és @return mezőket,
- a generált kommentek valóban segítik-e a kód megértését.

A tesztek alapján a tool egyszerűbb C++ fájlok esetén megfelelően működött, és használható kommenteket generált.

## Korlátok

A jelenlegi megoldás egy egyszerű MVP, ezért több korláttal rendelkezik:

- csak C++ fájlokra készült,
- nem használ valódi C++ szintaktikai elemzést,
- nagy fájloknál az API context window limitje problémát okozhat,
- az LLM válasza nem minden esetben tökéletesen determinisztikus,
előfordulhat, hogy a modell kisebb formázási módosításokat végez,
- nem integrálódik közvetlenül fejlesztőkörnyezetbe,
- nincs automatikus fájlfigyelés,
- nem kezeli külön a több fájlból álló projektek közötti kapcsolatokat.

## Jövőbeli fejlesztési lehetőségek

A projekt továbbfejlesztési lehetőségei:

- AST vagy parser alapú működés
- A program függvényenként vagy osztályonként dolgozná fel a kódot.
- VS Code extension
- A tool közvetlenül a fejlesztőkörnyezetből lenne használható.
- Automatikus fájlfigyelés (Ágens alapon)
- A program érzékelné a forrásfájl változását, és automatikusan frissítené a kommenteket.
- Több programozási nyelv támogatása
Például Python, Java, JavaScript vagy C# támogatása.
- Kommentminőség ellenőrzése
A program nemcsak generálná, hanem értékelné is a kommenteket.
- Lokális modellek támogatása
Kisebb projektek esetén API-költség nélkül is működhetne.
- Git integráció
Commit előtt automatikusan frissíthetné a dokumentációt.

## Érdekes tanulságok

A projekt elején többféle megközelítést is megvizsgáltam. Először lokális AI modellek futtatásával próbálkoztam Ollama segítségével, mert ez költséghatékony és adatvédelmi szempontból is előnyös megoldás lehetett volna. A gyakorlatban azonban hamar kiderült, hogy a rendelkezésre álló hardver nem elegendő ehhez a feladathoz. Még 12 GB VRAM mellett is problémát okozott a nagyobb forrásfájlok feldolgozása: a modellek könnyen túllépték a context window korlátait, vagy túl lassan adtak választ ahhoz, hogy a tool kényelmesen használható legyen.

Ezután kipróbáltam az OpenClaw nevű AI agent rendszert is. Az eredeti elképzelés az volt, hogy a feladatot egy autonóm agent segítségével oldom meg, amely képes lenne figyelni a fájlok változását, módosítani a kódot és akár további fejlesztői feladatokat is automatizálni. Ez technikailag érdekes iránynak bizonyult, de a konkrét feladathoz túl összetett megoldás lett volna. Egy egyszerű kódkommentelő tool miatt folyamatosan lokálisan vagy cloudban futtatni egy agentet feleslegesen bonyolult és költséges megközelítésnek tűnt.

A végső megoldás ezért egy egyszerűbb, célzottabb felépítés lett: egy Python alapú CLI tool, amely LLM API-t használ a C++ kód Doxygen-formátumú kommentelésére. A projekt tervezésében ChatGPT-t használtam, a script elkészítésében OpenAI Codex segített, a dokumentáció megírásához pedig szintén ChatGPT-t vettem igénybe. Magában a programban végül a Google Gemini API-ra támaszkodtam, mert jelenleg több modell is ingyenesen használható bizonyos korlátok között, és az eddigi tesztelés alapján a projektem nem éri el ezeket az API limiteket.

A félév során számomra az egyik legfontosabb tanulság az volt, hogy az LLM-ek és az AI fejlesztői eszközök nagyon gyorsan fejlődnek. Már a projekt készítése közben is több új modell és eszköz jelent meg, amelyek közül többet ki is próbáltam. Bár a választott feladat elsőre „csak” egy kódkommentelő programnak tűnt, valójában sokkal többet tanultam belőle: API-integrációról, prompt tervezésről, lokális és cloud alapú modellek közötti különbségekről, context window korlátokról, valamint arról, hogy mikor érdemes egy komplex agent rendszert használni, és mikor jobb egy egyszerűbb, célzott megoldás.

Összességében a projekt egyik legfontosabb mérnöki tanulsága az volt, hogy nem mindig a legösszetettebb technológia a legjobb választás. A kezdeti elképzelésekhez képest a végső megoldás egyszerűbb lett, de jobban illeszkedik a feladat tényleges céljához: egy gyorsan használható, könnyen bemutatható, működő AI-alapú forráskód-kommentelő eszköz készült.

## Összegzés

A projekt során elkészült egy egyszerű, de működőképes AI-alapú forráskód-kommentelő eszköz. A program C++ kódot dolgoz fel, és Doxygen-formátumú kommenteket generál hozzá LLM segítségével.

A megoldás jól bemutatja, hogyan lehet prompt engineering technikákkal szabályozni egy nyelvi modell viselkedését, és hogyan lehet egy általános LLM-et konkrét fejlesztői feladatra alkalmazni. Bár a tool jelenlegi formájában még nem production-ready, a projekt elérte az eredeti célját: demonstrálja az LLM-ek gyakorlati felhasználását forráskód dokumentálására.

A jövőben a megoldás továbbfejleszthető lenne fejlesztőkörnyezeti integrációval, automatikus fájlfigyeléssel, parser alapú kódelemzéssel és több programozási nyelv támogatásával.