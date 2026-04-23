Féléves feladat: Shadow Hunter - 3D Shooter

A program fordítása C nyelven, SDL2 és OpenGL könyvtárak felhasználásával történik.
Feladat helye: ...\demos\Feleves_Feladat
Mappaszerkezet:
    'src/': .c forrásfájlok
    'include/': .h fejlécfájlok
    'assets/': textúrák és modellek
Fordítás: A Makefile fordítja és futtatja a programot.

A "Shadow Hunter" egy FPS (belső nézetes) túlélő játék, amely egy sötét, ködös, procedurálisan generált erdőben játszódik. A játékos célja a túlélés és a rátámadó árnyéklények elpusztítása. Az ellenfelek a sötétségben sebezhetetlenek és áttetszőek. A játékosnak először meg kell világítania őket a zseblámpájával és csak ezután lehet őket fegyverrel megsebezni.

Procedurális generálás és Pálya:
A terep generálása egy fekete-fehér 'heightmap.png' képfájl beolvasásával történik, amely alapján a program legenerálja a talaj csúcspontjainak Y koordinátáit. Erre kerül ráfeszítésre egy ismétlődő textúra. A tereptárgyakat (fák, kövek) a program induláskor véletlenszerű koordinátákra (X, Z) szórja szét, és a magasságukat (Y) a heightmap adott pontjához igazítja.

Ellenfelek megjelenítése és Átlátszóság:
Az árnyéklények külön betöltött '.obj' modellek. Alapállapotban, a sötétben a 'GL_BLEND' funkció használatával félig áttetszőek és beleolvadnak a ködbe ('GL_FOG'). A játékosnak meg kell őket világítania a zseblámpájával. Ha az ellenfél belekerül a fénysugárba, a modell textúrát vált, az áttetszősége megszűnik, és sebezhetővé válik.

Mesterséges Intelligencia:
Az ellenfelek viselkedése egy egyszerűbb, vektor-matematikán alapuló követő algoritmusra épül. A fő ciklusban az ellenfelek folyamatosan kiszámítják a játékos aktuális pozíciója felé mutató irányvektort, és egyenes vonalban haladnak felé.

HUD:
A játékban 2D ortografikus vetítéssel ('glOrtho') egy HUD réteg kerül megjelenítésre. Ez tartalmazza:
    A célkeresztet a képernyő közepén.
    A játékos életerő szintjét.
    A zseblámpa akkumulátorának töltöttségi szintjét (ami a lámpa 3 különböző erősségi szintjének használatától függően merül).

Sebződés és Animációk:
Ellenfél: Találat esetén (AABB bounding box alapú ütközésvizsgálat a lövedék és a modell között) az ellenfél modellje egy pillanatra pirosan villan, halálakor pedig a modell a méretének csökkentésével eltűnik ("elfüstöl").
Játékos: Ha egy ellenfél AABB doboza metszi a játékosét, a képernyő széleire egy piros, átlátszó quadot rajzol a HUD, jelezve a sérülést.

Assets mappa: (Amint kész az összes textúra és modell beillesztem ide a letöltési linket.)

A felhasznált 3D modellek és textúrák forrásai:
