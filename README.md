Aye, a WiFi PIR motion sensor
=============================

This repository has the sources and enclosure design for a WiFi connected PIR motion sensor. The
sensor will send messages to a configured MQTT server. For now, all configuration is hard coded
in [WiFiConfiguration.hpp](include/WiFiConfiguration.hpp). The enclosure is in an OpenSCAD file
that will render into an [STL file](enclosure/AyeEnclosure.stl).