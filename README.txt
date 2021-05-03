ICP 2020/2021 Projekt - MQTT explorer

## Autori
    Michal Šlesár (xslesa01)
    Erik Belko (xbelko02)

## Popis aplikácie:

Uživateľ sa pomocou tejto aplikácie môže pripojiť na MQTT server verzie 5.ň

Po pripojení na server, má uživateľ možnosť manuálne prihlásiť odber konkrétnych tém, ktoré sa budú zobrazovať v hierarchickej forme. Uživateľ má možnosť prihlásiť si takúto tému podľa absolútneho názvu "tema/subtema/polozka", prípadne pomocou názvu relatívnemu voči aktuálne zvolenej téme v hierarchickom zozname. Táto funkcionalita sa ovláda jednoduchým checkboxom.
Položky v hierarchickom zozname označené modrou farbou predstavujú témy ktoré sú v daný čas odoberané, položky označené čiernou odoberané niesú.

Uživateľ má možnosť si v hierarchickom zozname označiť tému a následne k tejto téme publikovať správu. Táto správa môže byť zadaná manuálne pomocou textového poľa, alebo načítaná zo súboru.

Ak uživateľ odoberá nejakú tému, posledná prijatá správa k tejto téme sa zobrazuje v hierarchickom zozname pri názve témy. Ak je pozadie takejto správy žlté, správa bola odoslaná uživateľom, teda lokálne. Ak je pozadie biele, táto správa bola odoslaná iným uživateľom. Aplikácia sa pokúša každú správu spracovať ako obrázok, ak sa toto spracovanie nepodarí, namiesto obrázku je správa prijatá ako jednoduchý text.
Uživateľovi sa po označení témy v hierarchickom zozname načíta história prijatých správ. Táto história obsahuje v jednotlivých položkách čas prijatia danej správy a obsah danej správy. Ak je táto správa obrázok, namiesto správy sa ukáže text "Image". Uživateľ má možnosť si každú správu aj obrázok otvoriť v novom okne dvojitým kliknutím.

Uživateľ má možnosť uložiť snímok aktuálneho stavu do vybranej zložky, kde sa ku každej téme uloží správa ktorá bola prijatá ako posledná. V prípade obrázku sa táto správa uloží ako "payload.jpg". V opačnom prípade sa takáto správa uloží ako "payload.txt".

## Implementovaná funkcionalita
    - Pripájanie a odpájanie sa zo serveru
    - Sledovanie konkrétnych tém
    - Zobrazovanie tém v hierarchickej štruktúre, vrátane poslednej prijatej správy
    - Odosielanie textových správ a súborov k danej téme
    - Podpora obrázkov a textu
    - Uchovávanie histórie prijatých správ k danej téme
    - Otváranie správ v histórii v osobitnom okne
    - Farebne odlíšené sledované (modrá) a nesledované témy (čierna)
    - Farebne odlíšené odoslané správy (žlté pozadie) od prijatých (biele pozadie)
    - Ukladanie snímku aktuálneho stavu hierarchickej štruktúry tém a ich správ do zadanej zložky
    - Vytváranie dashboardu a jeho ukladanie do konfiguračného súboru
    - Načítavanie dashboardu z konfiguračného súboru
    - Pridávanie vstavaných widgetov do dashboardu a ich konfigurácia
    - Unikátna funkcionalita vstavaných widgetov
    - Simulácia prevádzky na danom serveri konfigurovateľná pomocou konfiguračného súboru

## Makefile
make build      -- preloží program a umiestní výsledný spustiteľný súbor do koreňovej zložky
make clean      -- vymaže dočasné súbory, ktoré vznikli počas prekladu
make doxygen    -- vygeneruje dokumentáciu
make pack       -- vytvorí archív so súbormi potrebnými pre odovzdanie
make run        -- preloží a spustí program
make            -- vymaže dočasné súbory a preloží program