OBS Studio <https://obsproject.com>
===================================

.. image:: https://github.com/Aeindus/obs-studio/actions/workflows/main.yml/badge.svg?branch=master&event=push
   :alt: OBS Studio Build Status - GitHub Actions
   :target: https://github.com/Aeindus/obs-studio/actions/workflows/main.yml?query=event%3Apush+branch%3Amaster

Customized version of obs. Master branch contains a number of improvements over "stable" release branch. For this reason all builds are made 
from the master branch.
Automatic updates disabled (do not update manually).

Documentatie
------------------

Cuvinte cheie cautate in numele scenei:
 * *JWScena*	Va cauta proiectorul aplicatiei JW Library si il va afisa pe al doilea monitor (indiferent de ce era prezentat inainte). Daca acest comportament creeaza probleme el poate fi dezactivat din *Settings > General > Projectors > Force JW projector on screen*.
 * *PScena*	Va afisa proiectorul creat de OBS (indiferent de ce era prezentat inainte).
 * *VScena*	Camera virtuala va fi dezactivata. Comportamentul normal este ca ea sa fie activa.
(poate fi folosita orice combinatie dintre **JW**, **P** si **V**)

Cand nu este folosita, camera virtuala afiseaza o imagine standard. Aceasta a fost inlocuita cu textul anului.
Pentru a modifica textul anului, updatati fisierul de aici: **plugins\\win-dshow\\data\\placeholder.png**. Nu e necesara recompilarea ci se poate face direct in folderul local. Cu toate acestea camera nu trebuie sa fie folosita de vreo aplicatie pentru ca imaginea sa fie actualizata.


.. table:: Obs scenes table
    :widths: auto
+--------------------------------+-------------------------------------------+-------------------------------------+-------------------------------------------------------------------------------------------------------+
| NUME SCENA                     | VIDEO                                     | AUDIO                               | DETALII                                                                                               |
+================================+===========================================+=====================================+=======================================================================================================+
| VJWScena 0 Premergatoare       |                                           |                                     | - Scena folosita inainte de inceperea intrunirii                                                      |
+--------------------------------+-------------------------------------------+-------------------------------------+-------------------------------------------------------------------------------------------------------+
| JWScena 1 Podium               | - Camera Sala                             | - Microfoane Sala                   | - Standard. Folosit in timpul intrunirii.                                                             |
|                                |                                           |                                     | - Se poate raspunde pe Zoom si din sala.                                                              |
+--------------------------------+-------------------------------------------+-------------------------------------+-------------------------------------------------------------------------------------------------------+
| JWScena 2 Prezentare JWLibrary | - Captura Monitor Secundar                | - Microfoane Sala                   | - Folosit pentru prezentarea unui video din JW.                                                       |
|                                | (Display Capture)                         | - Audio PC fara Zoom                | - Se poate raspunde pe Zoom si din sala.                                                              |
|                                |                                           | (application audio output capture)  | - Audio generat de Windows este transmis catre Zoom.                                                  |
+--------------------------------+-------------------------------------------+-------------------------------------+-------------------------------------------------------------------------------------------------------+
| PScena 3 Prezentare Media      | - Browser                                 | - Microfoane Sala                   | - Folosit pentru prezentarea multimedia (pdf/video/imagini).                                          |
|                                |                                           | - Audio PC fara Zoom                | - Se poate raspunde pe Zoom si din sala.                                                              |
|                                |                                           |                                     | - Audio generat de Windows este transmis catre Zoom.                                                  |
+--------------------------------+-------------------------------------------+-------------------------------------+-------------------------------------------------------------------------------------------------------+
| VPScena 4 Cuvantare de pe Zoom | - Captura proiector Zoom (Window Capture) |                                     | - Folosit pentru o cuvantare/demonstratie de pe Zoom.                                                 |
|                                | - Captura dashboard Zoom [**ascuns**]     |                                     | - Video-ul catre Zoom e inchis.                                                                       |
|                                |                                           |                                     | - Audio-ul catre Zoom e inchis.                                                                       |
+--------------------------------+-------------------------------------------+-------------------------------------+-------------------------------------------------------------------------------------------------------+
| PScena 5 Altele                | - Captura fereastra                       | - Microfoane Sala                   | - Folosit pentru orice alta necesitate: captura unei aplicatii, captura unei portiuni din ecran, etc. |
|                                |                                           |                                     | - Se poate raspunde pe Zoom si din sala.                                                              |
+--------------------------------+-------------------------------------------+-------------------------------------+-------------------------------------------------------------------------------------------------------+

* **JWScena 2**

  * *Captura Monitor Secundar* trebuie sa foloseasca ca metoda de captura *DXGI Desktop Duplication*. Altfel va aparea o bordura galbena pe marginea monitorului

* **PScena 3**

  * Sursa de imagini ascunsa este pe post de backup. Unele formate de imagini si de video nu pot fi redate prin browser asadar pentru cazurile exceptionale va fi folosita aceasta sursa. Totodata, sursa de video *VLC Slideshow* nu poate fi folosita ca backup deoarece nu reda audio-ul prin difuzoare ci doar prin mixerul din OBS.
  * Va fi creat un folder in desktop care va fi adaugat ca sursa de materiale la *Browser* si *Imagini*.
  * Configuratie *Browser*:
  
    * *width:1920px* si *height:1080px*.
    * activata optiunea *Shutdown source when not visible*. Altfel orice video sau audio continua sa se fie redat chiar si cand se schimba scena cu alta.    

* **VPScena 4** 

  * Prima sursa va captura fereastra a 3-a a Zoomului. In lista de procese apare intotdeauna un proces numit *[Zoom] Projector Window* care trebuie selectat. Insa daca optiunea *Use Dual Monitors* nu a fost activata inainte de inceperea sesiunii atunci proiectorul din Zoom nu mai poate fi deschis (limitare a Zoom-ului). In acest caz se va apela la a doua captura care va inregistra fereastra de dashboard a Zoom-ului.
  * A doua captura ascunsa e pe post de backup si inregistreaza fereastra de dashboard a Zoom-ului. Partea de jos a capturii poate fi taiata prin crop pentru a ascunde controalele.
  * La ambele surse metoda de captare trebuie setata in proprietati la *Windows 10 (1903 and up)*.
  
* **PScena 5**

  * Folosit pentru orice eventualitate. Se pot captura portiuni din ecran, audio de la alte aplicatii, surse video, camere, etc.
  
|

* Sursa **Browser** a fost modificata. Se pot afisa fisiere, foldere sau url-uri. Videoclipurile pot fi pornite sau puse pe pauza cu ajutorul butonului din bara de control (Next, Stop, Previous). Numele fisierului in derulare este afisat tot in bara. Pentru pdf-uri se poate reactiva bordura de sus a editorului din proprietati (optiunea *Show pdf toolbar*).

* **Audio PC fara Zoom** foloseste pluginul extern *Application Audio Output Capture*. Sursa audio **trebuie** sa excluda *Zoom.exe* si *obs64.exe*.

* **Settings > Advanced > General > Show active outputs warning on exit** trebuie setat pe fals. Altfel daca camera virtuala e folosita OBS va afisa intotdeauna un dialog "Are you sure?" la inchidere.

Development
-------------------

Submodules are hard to use. If after a merge there are modified/changed files belonging to a submodule then follow these instructions: https://stackoverflow.com/a/32581852/3971417.

The folder plugins/win-capture-audio contains a compiled version of `win-capture-audio <https://github.com/bozbez/win-capture-audio/>`_. The files within must be replaced from time to time (copy-paste).

What is OBS Studio?
-------------------

OBS Studio is software designed for capturing, compositing, encoding,
recording, and streaming video content, efficiently.

It's distributed under the GNU General Public License v2 (or any later
version) - see the accompanying COPYING file for more details.

Quick Links
-----------

- Website: https://obsproject.com

- Help/Documentation/Guides: https://github.com/obsproject/obs-studio/wiki

- Forums: https://obsproject.com/forum/

- Build Instructions: https://github.com/obsproject/obs-studio/wiki/Install-Instructions

- Developer/API Documentation: https://obsproject.com/docs

- Donating/backing/sponsoring: https://obsproject.com/contribute

- Bug Tracker: https://github.com/obsproject/obs-studio/issues
