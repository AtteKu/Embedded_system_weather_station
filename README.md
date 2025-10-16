Projektin tarkoituksena oli rakentaa toimiva MQTT-client osaksi sääasemajärjestelmää.
Projektissa käytettiin MQTT-protokollaan perustuvaa tiedonsiirtomallia. 
Rakennettu kytkentä, sensori ja Arduino Nano toimivat MQTT-clientina. Tässä 
tapauksessa MQTT-client lähetti valoisuus- tai tuulennopeusdatan Ethernet-kytkimen ja välittäjänä
toimivan Raspberry Pi:n kautta tietokantaan JSON-muodossa.
Tämän jälkeen dataa voitiin tarkastella verkkokäyttöliittymässä. 

Projektissa suunniteltiin ja rakennettiin laitteisto, joka pystyi käsittelemään sekä 
analogista (valoisuus) että digitaalista (tuulen nopeus) signaalia, joita saatiin 
valo- ja tuulennopeusantureilta. Nämä signaalit siirrettiin rakennettujen 
laitteistojen kautta Arduino Nanolle. 

Lisäksi suunniteltiin ja toteutettiin Arduinoilla toimiva ohjelmisto, jonka avulla 
saatu data voitiin lähettää tietokantaan riippumatta signaalin tyypistä. Kokonaisuutena
projekti tuotti toimivan laitteisto- ja ohjelmistokokonaisuuden osaksi sääasemajärjestelmää. 

Tavoitteena oli oppia projektityöskentelyä sekä suunnitella ja toteuttaa käytännössä
niin laitteisto kuin ohjelmisto. Projektissa keskityttiin erityisesti tiedonsiirron 
toteutukseen MQTT:n avulla sekä antureiden datan käsittelyyn ja välittämiseen 
tietokantaan. Muita mahdollisia sääasemakomponentteja, kuten kosteus- tai
lämpötila-antureita, ei tässä projektissa käsitelty.
