Projektin tarkoituksena oli rakentaa toimiva MQTT-client osaksi sääasemajärjes
telmää. Projektissa käytettiin MQTT-protokollaan perustuvaa tiedonsiirtomallia. 
Rakennettu kytkentä, sensori ja Arduino Nano toimivat MQTT-clientina. Tässä 
tapauksessa MQTT-client lähetti valoisuus- tai tuulennopeusdatan Ethernet-kyt
kimen ja välittäjänä toimivan Raspberry Pi:n kautta tietokantaan JSON-muo
dossa. Tämän jälkeen dataa voitiin tarkastella verkkokäyttöliittymässä. 

Projektissa suunniteltiin ja rakennettiin laitteisto, joka pystyi käsittelemään sekä 
analogista (valoisuus) että digitaalista (tuulen nopeus) signaalia, joita saatiin 
valo- ja tuulennopeusantureilta. Nämä signaalit siirrettiin rakennettujen laitteisto
jen kautta Arduino Nanolle. 

Lisäksi suunniteltiin ja toteutettiin Arduinoilla toimiva ohjelmisto, jonka avulla 
saatu data voitiin lähettää tietokantaan riippumatta signaalin tyypistä. Kokonai
suutena projekti tuotti toimivan laitteisto- ja ohjelmistokokonaisuuden osaksi sää
asemajärjestelmää. 

Tavoitteena oli oppia projektityöskentelyä sekä suunnitella ja toteuttaa käytän
nössä niin laitteisto kuin ohjelmisto. Projektissa keskityttiin erityisesti tiedonsiirron 
toteutukseen MQTT:n avulla sekä antureiden datan käsittelyyn ja välittämiseen 
tietokantaan. Muita mahdollisia sääasemakomponentteja, kuten kosteus- tai läm
pötila-antureita, ei tässä projektissa käsitelty.
