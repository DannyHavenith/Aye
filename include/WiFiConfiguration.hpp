//
//  Copyright (C) 2020 Danny Havenith
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SRC_WIFICONFIGURATION_HPP_
#define SRC_WIFICONFIGURATION_HPP_

namespace
{
	// credentials to log on to WiFi network
	constexpr auto networkSID = "<yourSSIDHere>";
	constexpr auto networkPassword = "<yourPasswordHere>;

	// host name AND access point name
	constexpr auto myName = "Aye001";

	constexpr auto mqttServer = "<yourMQTTServernameHere>";
	constexpr auto mqttPort = 1883;

}

#endif /* SRC_WIFICONFIGURATION_HPP_ */
