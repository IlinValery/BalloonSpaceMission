#pragma once

bool initRTC(){
	Rtc.Begin();
	RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
	if (!Rtc.IsDateTimeValid()) {
	// Common Causes:
	//    1) first time you ran and the device wasn't running yet
	//    2) the battery on the device is low or even missing

		Serial.println("RTC lost confidence in the DateTime!");
		//Rtc.SetDateTime(compiled);
	}

	if (Rtc.GetIsWriteProtected()){
		Serial.println("RTC was write protected, enabling writing now");
		Rtc.SetIsWriteProtected(false);
	}

	if (!Rtc.GetIsRunning()) {
		Serial.println("RTC was not actively running, starting now");
		Rtc.SetIsRunning(true);
	}

	RtcDateTime now = Rtc.GetDateTime();
	
	if (!now.IsValid()){
		return false;
	}
	return true;
}
