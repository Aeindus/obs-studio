OBS Studio <https://obsproject.com>
===================================

.. image:: https://github.com/Aeindus/obs-studio/actions/workflows/main.yml/badge.svg?branch=master&event=push
   :alt: OBS Studio Build Status - GitHub Actions
   :target: https://github.com/Aeindus/obs-studio/actions/workflows/main.yml?query=event%3Apush+branch%3Amaster

Customized version of obs. Master branch contains a number of improvments over "stable" release branch. For this reason all builds are made 
from the master branch.
Automatic updates disabled (do not update manually).

Documentatie
------------------

Cuvinte cheie cautate in numele scenei:
 - *JWScena*	Va cauta proiectorul aplicatiei JW Library si il va afisa pe al doilea monitor (indiferent de ce era prezentat inainte)
 - *PScena*	Va afisa proiectorul creat de OBS (indiferent de ce era prezentat inainte).
 - *VScena*	Camera virtuala va fi dezactivata. Comportamentul normal este ca ea sa fie activa.
 
Cand nu este folosita, camera virtuala afiseaza o imagine standard. Aceasta a fost inlocuita cu textul anului.
Pentru a modifica textul anului, updatati fisierul de aici: **plugins\\win-dshow\\data\\placeholder.png** si recompilati.

.. table:: Obs scenes table
    :widths: auto
+--------------------------------+--------------------------------------------+----------------------------------------------------------+---------------------------------------------------------------------+
| NUME SCENA                     | VIDEO                                      | AUDIO                                                    | DETALII                                                             |
+================================+============================================+==========================================================+=====================================================================+
| JWScena Podium                 | - Camera                                   | - Microfoane sala                                        | - Standard. Folosit in timpul intrunirii.                           |
+--------------------------------+--------------------------------------------+----------------------------------------------------------+---------------------------------------------------------------------+
| JWScena 2 Videoclip pe Zoom    | - Captura Ecran Secundar (Display Capture) | - Microfoane sala                                        | - Folosit pentru prezentarea unui video din JW.                     |
|                                |                                            | - Audio PC fara Zoom (application audio output capture)  | - Se poate raspunde pe Zoom si din sala.                            |
|                                |                                            |                                                          | - Audio generat de Windows este transmis catre Zoom.                |
+--------------------------------+--------------------------------------------+----------------------------------------------------------+---------------------------------------------------------------------+
| PScena 3 Prezentari Imagini    | - Image Slideshow (Image Slideshow)        | - Microfoane sala                                        | - Folosit pentru prezentarea de imagini dintr-un folder predefinit. |
|                                | - Media Source (Media Source)              |                                                          | - Se poate raspunde pe Zoom si din sala.                            |
+--------------------------------+--------------------------------------------+----------------------------------------------------------+---------------------------------------------------------------------+
| PScena 4 Prezentare pdf/media  | - Browser (Browser)                        | - Microfoane sala                                        | - Folosit pentru prezentarea multimedia (pdf/surse video).          |
|                                |                                            | - Audio PC fara Zoom                                     | - Se poate raspunde pe Zoom si din sala.                            |
|                                |                                            |                                                          | - Audio generat de Windows este transmis catre Zoom.                |
+--------------------------------+--------------------------------------------+----------------------------------------------------------+---------------------------------------------------------------------+
| VPScena 5 Cuvantare de pe Zoom | - Captura fereastra Zoom (Window Capture)  |                                                          | - Folosit pentru o cuvantare/demonstratie de pe Zoom.               |
|                                |                                            |                                                          | - Video-ul catre Zoom e inchis.                                     |
|                                |                                            |                                                          | - Audio-ul catre Zoom e inchis.                                     |
+--------------------------------+--------------------------------------------+----------------------------------------------------------+---------------------------------------------------------------------+
|                                |                                            |                                                          |                                                                     |
+--------------------------------+--------------------------------------------+----------------------------------------------------------+---------------------------------------------------------------------+

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

Contributing
------------

- If you would like to help fund or sponsor the project, you can do so
  via `Patreon <https://www.patreon.com/obsproject>`_, `OpenCollective
  <https://opencollective.com/obsproject>`_, or `PayPal
  <https://www.paypal.me/obsproject>`_.  See our `contribute page
  <https://obsproject.com/contribute>`_ for more information.

- If you wish to contribute code to the project, please make sure to
  read the coding and commit guidelines:
  https://github.com/obsproject/obs-studio/blob/master/CONTRIBUTING.rst

- Developer/API documentation can be found here:
  https://obsproject.com/docs

- If you wish to contribute translations, do not submit pull requests.
  Instead, please use Crowdin.  For more information read this thread:
  https://obsproject.com/forum/threads/how-to-contribute-translations-for-obs.16327/

- Other ways to contribute are by helping people out with support on
  our forums or in our community chat.  Please limit support to topics
  you fully understand -- bad advice is worse than no advice.  When it
  comes to something that you don't fully know or understand, please
  defer to the official help or official channels.
