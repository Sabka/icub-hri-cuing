Zdrojový kód v tomto repozitári je súčasťou projektu zameriavajúceho sa na skúmanie efektu sociálneho pohľadu v interakcii človeka a robota. Kód pracuje s robotom iCub v dvoch formách stelesnenia.
Kód je vyvinutý, testovaný a používaný na opereačnom systéme Ubuntu 20.04.4 LTS (Focal Fossa).


Repozitár obsahuje :
- model SDF sveta používaný pre vývoj pomocou simulátora Gazebo
- priečinok with-lights - súčasťou vyvinutého prostredia bolo hardvérové zariadenie na vytváranie svetelných stimulov. Tento priečinok obsahuje zdrojové kódy, ktoré vyžadujú pripojenie tohto zariadenia pomocou USB.
- priečinok without-lights - obsahuje podobné zdrojové kódy ako priečinok with-lights, s tým rozdielom, že nevyžadujú pripojenie hardvérového zariadenia na vytváranie svetelných stimulov, svetelné stimuly sú simulované výpisom na konzolu. 
- priečinok results, ktorý obsahuje dáta namerané týmto softvérom, pre účely skúmania psychologického efektu sociálneho pohľadu


Spúšťanie:

Zvolíme si, či chceme pracovať so skutočnými svetlami - priečinkom with-lights alebo len so svetlami v podobe výpisov na konzolu - priečinkom without-lights. Vrámci priečinka si zvolíme, či chceme pracovať s robotom v simulátore - priečinok simulator, alebo s fyzickým robotom - priečinok physical.

Program v zvolenom priečinku je potrebné skompilovať pomocou CMake. 
Následne je program spúšťaný z terminálového okna na operačnom systéme 
Ubuntu 20.04.4 LTS (Focal Fossa). Pri spúšťaní je treba
ako parameter zadať meno robota. To je prefix názvov portov pripojených na YARP
serveri, napríklad icubSim ak máme spustený simulátor. 
Program potom spustíme pomocou ./<názov skompilovaného spustiteľného súboru> --robot <meno robota>, 
  teda napríklad ./simulator --robot icubSim. 
  
Ak je program spúšťaný správne, pár sekúnd načítava encodery, o
čom nás aj notifikuje a následne sa objaví výzva na zadanie identifikačného čísla účast-
níka. Po zadaní a potvrdení klávesou Enter začne prebiehať experiment skladajúci sa
z 80 meraní reakčného času, teda iterácií
