Féléves feladat: Shadow Hunter - 3D Shooter

A program fordítása C nyelven, SDL2 és OpenGL könyvtárak felhasználásával történik.
Feladat helye: ...\demos\Feleves_Feladat
Mappaszerkezet:
    'src/': .c forrásfájlok
    'include/': .h fejlécfájlok
    'assets/': textúrák és modellek
        'assets/models/'
        'assets/textures/'

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

Assets mappa: https://drive.google.com/file/d/1zpFEsWtNti2CTzNEya_pqDx7AWDPD7Eo/view?usp=sharing

A felhasznált modellek és textúrák forrásai:
https://opengameart.org/content/seamless-grass-texture-1
https://www.magnific.com/free-photo/white-crumpled-paper-background-simple-diy-craft_17434094.htm#fromView=keyword&page=1&position=23&uuid=346f17cf-5421-4daf-abac-4a9251d7e12d&query=Paper+texture
https://manticorp.github.io/unrealheightmap/#latitude/0/longitude/0/zoom/5/outputzoom/13/width/128/height/128/outputformat/png16
https://brokenvector.itch.io/low-poly-rock-pack
https://kenney.nl/assets/nature-kit
https://skfb.ly/6ZnzL