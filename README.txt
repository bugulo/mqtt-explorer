ICP 2020/2021 Projekt - MQTT explorer

## Autori
    Michal Šlesár (xslesa01)
    Erik Belko (xbelko02)

## Popis aplikácie:

Uživateľ sa pomocou tejto aplikácie môže pripojiť na MQTT server verzie 5.0, sledovať rôzne témy a prijímať a odosielaľ k nim textové a súborové správy.
Je potrebné špecifikovať konkrétne témy pre odoberanie, podľa absolútneho názvu "tema/subtema/polozka", prípadne pomocou názvu relatívnemu voči aktuálne zvolenej téme v hierarchickom zozname. Táto funkcionalita sa ovláda jednoduchým checkboxom.
Tieto témy sú následne zobrazované v hierarchickej štruktúre a klient k týmto témam prijíma správy. Poslednú správu zobrazuje v hierarchickej štruktúre tém. 
Staršie správy sú uložené v histórii správ danej témy, ktorá môže byť obmedzená spúšťacím parametrom --history.
Ak je pozadie prijatej správy žlté, správa bola odoslaná uživateľom, teda lokálne. Ak je pozadie biele, táto správa bola odoslaná iným uživateľom.
Položky v hierarchickom zozname označené modrou farbou predstavujú témy, ktoré sú v daný čas odoberané. Položky označené čiernou odoberané niesú.
Aplikácia sa pokúša každú správu spracovať ako obrázok, ak sa toto spracovanie nepodarí, namiesto obrázku je správa prijatá ako jednoduchý text.
Po označení témy v hierarchickom zozname tém, sa uživateľovi načíta história prijatých správ pre túto tému a taktiež môže k tejto téme publikovať správu. Táto správa môže byť zadaná manuálne pomocou textového poľa, alebo načítaná zo súboru.
História správ pre danú tému obsahuje v jednotlivých položkách čas prijatia danej správy a obsah danej správy. Ak je táto správa obrázok, namiesto správy sa ukáže text "Image". Uživateľ má možnosť si každú správu aj obrázok otvoriť v novom okne dvojitým kliknutím.
Uživateľ má možnosť uložiť snímok aktuálneho stavu do vybranej zložky, kde sa ku každej téme uloží správa ktorá bola prijatá ako posledná. V prípade obrázku sa táto správa uloží ako "payload.jpg". V opačnom prípade sa takáto správa uloží ako "payload.txt".

V časti dashboard je možné pridávať widgety a priradzovať ich konkrétnym témam. Widget ponúka jednoduchú komunikáciu s daným topicom.
Konfiguráciu dashboardu je možné uložiť do uživateľom špecifikovaného súboru a z takéhoto súboru je možné dashboard opäť načítať.

V hlavnom okne je možné zapnúť simulátor reálnej trafiky na serveri, na ktorom je pripojený a kedykoľvek tento simulátor opäť vypnúť. 
Pri prvom zapnutí simulátoru musí uživateľ špecifikovať konfiguračný súbor, obsahujúci zoznam simulovaných tém a správ.

## Demo
    demo_subscribe.gif - Demonštruje funkcionalitu subscribovania topicov
    demo_dashboard.gif - Demonštruje funkcionalitu dashboadu
    demo_simulator.gif - Demonštruje funkcionalitu simulátoru

    examples/dashboard.json - Ukážkový konfiguračný súbor pre dashboard, je možné ho načítať v aplikácii
    examples/simulator.json - Ukážkový konfiguračný súbor pre simulátor, je možné ho načítať pri prvom spustení

## Implementovaná funkcionalita
    - Pripájanie a odpájanie sa zo serveru
    - Sledovanie konkrétnych tém absolútne aj relatívne (Wildcardy niesú podporované)
    - Zobrazovanie tém v hierarchickej štruktúre, vrátane posledne prijatej správy
    - Odosielanie textových správ a súborov k danej téme
    - Podpora zobrazovania textu a obrázkov (v osobitnom okne)
    - Uchovávanie histórie prijatých správ k danej téme (možnosť obmedziť tento počet pomocou spúšťacieho parametru)
    - Otváranie dlhých správ v histórii v osobitnom okne
    - Farebne odlíšené sledované (modrá) a nesledované témy (čierna)
    - Farebne odlíšené odoslané správy (žlté pozadie) od prijatých (biele pozadie)
    - Ukladanie snímku aktuálneho stavu hierarchickej štruktúry tém a ich správ do zadanej zložky
    - Vytváranie dashboardu a jeho ukladanie do konfiguračného súboru
    - Načítavanie dashboardu z konfiguračného súboru
    - Pridávanie vstavaných widgetov do dashboardu a ich konfigurácia
    - Unikátna funkcionalita vstavaných widgetov
    - Simulácia prevádzky na danom serveri konfigurovateľná pomocou konfiguračného súboru

## Spúštacie parametre
    -h, --history <number> - Obmedzí maximálny počet správ uložených vrámci histórie prijatých správ témy (Default: 10)

## Makefile
    make build      -- preloží program a umiestní výsledný spustiteľný súbor do koreňovej zložky
    make clean      -- vymaže dočasné súbory, ktoré vznikli počas prekladu
    make doxygen    -- vygeneruje dokumentáciu
    make pack       -- vytvorí archív so súbormi potrebnými pre odovzdanie
    make run        -- preloží a spustí program
    make            -- vymaže dočasné súbory a preloží program

## Príklad spustenia
    ./mqtt-explorer
    ./mqtt-explorer -h 5
    ./mqtt-explorer --history 5

## Použité knižnice
    QT v5.12.8
    QMake v3.1
    Eclipse Paho MQTT C library v1.3.8
    Eclipse Paho MQTT C++ library v1.2.0

## Použitý kód tretej strany
    FlowLayout z https://code.qt.io/cgit/qt/qtbase.git/tree/examples/widgets/layouts/flowlayout/flowlayout.h?h=5.12