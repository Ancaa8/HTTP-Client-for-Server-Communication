# HTTP-Client-for-Server-Communication
A client application that connects to a server and executes HTTP requests (GET, POST, DELETE). It manages authentication, handles JSON data using the Parson library, and includes additional commands for exporting/importing books.

Prezinta o aplicate client care se conecteaza la un server si executa comenzi de tip GET, POST, DELETE.

Construirea request-urilor

-Am facut o functie generalizata (compute_http_request) ce primeste ca si parametru un Enum (HttpMethod { GET, POST, DELETE }) si restul elementelor necesare pentru a construi un request, cele inexistentente fiind setate la NULL.
-Am implementat o functie (send_http_request), care porneste o conexiune, trimite mesajul folosind functia din laborator si returneaza raspunsul primit de la server.

Logica client

-Am setat userul, parola, tokenul si cookie-ul in variabile globale pentru a le folosi in toate request-urile.
-Folosind fgets citesc pe rand comenzile si iputurile necesare pentru a le executa.
-Am folosit biblioteca parson pentru a manipula JSON-ul. Am folosit-o pentru a crea body-ul request-urilor de tip POST

login() - trimite un request de tip POST cu userul si parola si seteaza cookie-ul.

register() - trimite un request de tip POST cu userul si parola.

enter_library() - trimite un request de tip GET si seteaza token-ul.

get_books() - trimite un request de tip GET si afiseaza toate cartile folosind o functie pentru pretty print.

get_book() - trimite un request de tip GET cu id-ul cartii si afiseaza cartea folosind o functie pentru pretty print.

add_book() - trimite un request de tip POST cu toate datele cartii.

delete_book() - trimite un request de tip DELETE cu id-ul cartii.

logout() - trimite un request de tip GET si sterge cookie-ul si token-ul.

exit() - inchide aplicatia.

-Am implementat si niste comenzi extra, deoarece mi s-au parut folositoare in testarea aplicatiei, cat si utilizare.

export_books() - exporta toate cartile intr-un fisier JSON cu numele "books.json".

export_book() - exporta o carte specifica intr-un fisier JSON cu numele cartii.

import_book() - importa o carte dintr-un fisier JSON primit ca si parametru.

-Funtiile in caz de eroare de la server, afiseaza si un mesaj de eroare specific cat si eroarea primita de la server.

-Am incercat sa tratez toate erorile posibile, de ex, intrare in librarie nelogat, si alte comenzi care nu pot fi executate fara a fi logat sau fara a fi in librarie. Am verificat ca username-ul sa nu contina spatii, iar page-count-ul sa fie un numar.
